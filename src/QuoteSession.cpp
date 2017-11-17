#include <map>
#include "../include/QuoteSession.h"
#include "tradeFuncID.h"
#include "../include/Config.h"
#include "../include/XhtGetwayApp.h"


CQuoteSession::CQuoteSession(boost::asio::io_service& io_service, CMyServer &server, bool bTradeSession/* = true*/)
	: m_socket(io_service),
	m_server(server),
	m_reqSema(0),
	m_resSema(0)
{
	m_pQuoteSpi = new MyQuoteSpi(*this);
}

void CQuoteSession::handle_read_header(const boost::system::error_code& error)
{
	if (!error)
	{
		int nSize = atoi(m_szBuf);
		memset(m_szBuf, 0, BUF_SIZE);
		boost::asio::async_read(m_socket,
			boost::asio::buffer(m_szBuf, nSize),
			boost::bind(&CQuoteSession::handle_read_body, shared_from_this(), boost::asio::placeholders::error));
	}
	else
	{
		m_server.leave_quote(shared_from_this());
	}
}

void CQuoteSession::handle_read_body(const boost::system::error_code& error)
{
	if (!error)
	{
		std::string strPacket = m_szBuf;
		
		// 向接收队列保存接收的请求
		if (pushReqPacket(strPacket))
		{
			triggerReqSignal();
		}

		memset(m_szBuf, 0, BUF_SIZE);

		boost::asio::async_read(m_socket,
			boost::asio::buffer(m_szBuf, HEADER_LENGTH),
			boost::bind(&CQuoteSession::handle_read_header, shared_from_this(), boost::asio::placeholders::error));
	}
	else
	{
		m_server.leave_quote(shared_from_this());
	}
}

void CQuoteSession::handle_write(const boost::system::error_code& error)
{
	if (!error)
	{
		// 在这里删除队列中发送成功的消息
		if (!m_quote_res_queue.empty())
		{
			m_quote_res_queue.pop_front();
		}

		// 判断是否还有发送的响应包
		if (!m_quote_res_queue.empty())
		{
			std::string &strMsg = m_quote_res_queue.front();
			boost::asio::async_write(m_socket,
				boost::asio::buffer(strMsg, strMsg.length()),
				boost::bind(&CQuoteSession::handle_write, shared_from_this(),boost::asio::placeholders::error));
		}
	}
	else
	{
		m_server.leave_quote(shared_from_this());
	}
}

void CQuoteSession::thread_handler_req(CQuoteSession * pThis)
{
	std::string strPacket;
	while (true)
	{
		pThis->m_reqSema.wait();
		strPacket = "";
		if (pThis->popReqPacket(strPacket))
		{
			pThis->handle_req(strPacket);
		}
	}
}

void CQuoteSession::thread_handler_res(CQuoteSession * pThis)
{
	std::string strPacket;
	while (true)
	{
		pThis->m_resSema.wait();
		strPacket = "";
		if (pThis->popResPacket(strPacket))
		{
			pThis->send(strPacket);
		}
	}
}

void CQuoteSession::handle_req(std::string strPacket)
{
	Json::Value vRoot;
	if (m_reader.parse(strPacket, vRoot))
	{
		Json::Value vHead = vRoot["head"];
		std::string strTradeCode = vHead["s_trdcode"].asString();

		if (strTradeCode.compare(TS_FUNC_QUOTE_SUB) == 0)// 行情订阅
		{
			handle_req_quote_sub(vRoot);
		}
		else if (strTradeCode.compare(TS_FUNC_CANCEL_QUOTE_SUB) == 0)// 取消行情订阅
		{
			handle_req_cancel_quote_sub(vRoot);
		}
	}
	else// json串解析出错
	{
		//
	}
}

void CQuoteSession::handle_req_quote_sub(Json::Value & v)
{
	Json::Value vHead = v["head"];
	std::string strPacketID = vHead["s_trdid"].asString();

	Json::Value vBody = v["body"];
	std::string strStocks = vBody["stocks"].asString();

	if (!m_quoteReq.doSubQuote(strStocks.c_str()))
	{
		send_error_res_packet(strPacketID, TS_FUNC_ASSET_DETAIL, TS_RES_CODE_SUB_QUOTE_FAILED, "股票订阅失败");
	}
}

void CQuoteSession::handle_req_cancel_quote_sub(Json::Value & v)
{
	Json::Value vHead = v["head"];
	std::string strPacketID = vHead["s_trdid"].asString();

	Json::Value vBody = v["body"];
	std::string strStocks = vBody["stocks"].asString();

	if (!m_quoteReq.doUnSubQuote(strStocks.c_str()))
	{
		send_error_res_packet(strPacketID, TS_FUNC_ASSET_DETAIL, TS_RES_CODE_UNSUB_QUOTE_FAILED, "取消股票订阅失败");
	}
}

void CQuoteSession::combo_head(Json::Value &vHead, std::string strPacketID, std::string strTradeCode, std::string strResCode, std::string strMsg)
{
	time_t tt = time(NULL);
	tm* t = localtime(&tt);
	char szTime[32];
	sprintf(szTime, "%d%02d%02d%02d%02d%02d",
		t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
		t->tm_hour, t->tm_min, t->tm_sec);

	vHead["s_trd"] = strPacketID;
	vHead["s_trdcode"] = strTradeCode;
	vHead["s_trddatetime"] = szTime;
	vHead["s_respcode"] = strResCode;
	vHead["s_respdesc"] = strMsg;
}

bool CQuoteSession::pushReqPacket(std::string &strPacket)
{
	boost::mutex::scoped_lock lock(m_mutexReq);
	if (!strPacket.empty())
	{
		m_quote_req_queue.push_back(strPacket);

		return true;
	}

	return false;
}

bool CQuoteSession::popReqPacket(std::string & strPacket)
{
	boost::mutex::scoped_lock lock(m_mutexReq);

	if (!m_quote_req_queue.empty())
	{
		std::string &strMsg = m_quote_req_queue.front();
		if (strMsg.length() > 0)
		{
			strPacket = strMsg;
			m_quote_req_queue.pop_front();

			return true;
		}

		return false;
	}

	return false;
}

void CQuoteSession::pushResPacket(std::string &strPacket)
{
	boost::mutex::scoped_lock lock(m_mutexRes);

	m_quote_res_queue.push_back(strPacket);

}

bool CQuoteSession::popResPacket(std::string &strPacket)
{
	boost::mutex::scoped_lock lock(m_mutexRes);

	if (!m_quote_res_queue.empty())
	{
		strPacket = m_quote_res_queue.front();
		m_quote_res_queue.pop_front();

		return true;
	}

	return false;
}

void CQuoteSession::triggerReqSignal()
{
	m_reqSema.signal();
}

void CQuoteSession::triggerResSignal()
{
	m_resSema.signal();
}

void CQuoteSession::send_error_res_packet(std::string strPacketID, std::string strTradeCode, std::string strResCode, std::string strMsg)
{
	Json::Value vHeadRes;
	combo_head(vHeadRes, strPacketID, strTradeCode, strResCode, strMsg);

	Json::Value vRes;
	vRes["head"] = vHeadRes;

	Json::FastWriter writer;
	std::string &strJson = writer.write(vRes);

	send(strJson);
}

uint64_t CQuoteSession::getSessionID()
{
	return m_nSessionID;
}

void CQuoteSession::start()
{
	static int nSessionID = 0;
	++nSessionID;
	m_nSessionID = nSessionID;

	// 连接xtp行情服务器
	{
		T_XtpQuoteLogin xtpQuoteServer;
		theApp.config.getXtpQuoteInfo(xtpQuoteServer, 0);

		T_XtpInitInfo xtpInitInfo;
		int nClientID;
		theApp.config.getQuoteInit(xtpInitInfo, nClientID, 0);
		m_quoteReq.init(nClientID, xtpInitInfo.szCatalog, m_pQuoteSpi);

		if (!m_quoteReq.doLogin(xtpQuoteServer.szUserName, xtpQuoteServer.szPwd, xtpQuoteServer.tAddr.szIP, xtpQuoteServer.tAddr.nPort))
		{
			printf("session：%d, xtp行情服务器连接失败!\n", m_nSessionID);
		}
	}

	// 创建监听线程
	boost::thread thReq(boost::bind(CQuoteSession::thread_handler_req, this));
	boost::thread thRes(boost::bind(CQuoteSession::thread_handler_res, this));
	

	// 将当前会话实例添加到队列中
	m_server.join_quote(shared_from_this());

	boost::asio::async_read(m_socket,
		boost::asio::buffer(m_szBuf, HEADER_LENGTH),
		boost::bind(&CQuoteSession::handle_read_header, shared_from_this(), boost::asio::placeholders::error));
}

void CQuoteSession::send(const std::string& msg)
{
	bool write_in_progress = !m_quote_res_queue.empty();
	m_quote_res_queue.push_back(msg);
	if (!write_in_progress)
	{
		std::string &strMsg = m_quote_res_queue.front();

		boost::asio::async_write(m_socket,
			boost::asio::buffer(strMsg, strMsg.length()),
			boost::bind(&CQuoteSession::handle_write, shared_from_this(), boost::asio::placeholders::error));
	}
}

boost::asio::ip::tcp::socket& CQuoteSession::get_socket()
{
	return m_socket;
}