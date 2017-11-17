#include <map>
#include "../include/TradeSession.h"
#include "tradeFuncID.h"
#include "../include/Config.h"
#include "../include/XhtGetwayApp.h"


CTradeSession::CTradeSession(boost::asio::io_service& io_service, CMyServer &server, bool bTradeSession/* = true*/)
	: m_socket(io_service),
	m_server(server),
	m_reqSema(0),
	m_resSema(0)
{
	m_pTradeSpi = new CXtpTraderRes(m_tradeReq, *this);
}

void CTradeSession::handle_read_header(const boost::system::error_code& error)
{
	if (!error)
	{
		int nSize = atoi(m_szBuf);
		memset(m_szBuf, 0, BUF_SIZE);
		boost::asio::async_read(m_socket,
			boost::asio::buffer(m_szBuf, nSize),
			boost::bind(&CTradeSession::handle_read_body, shared_from_this(), boost::asio::placeholders::error));
	}
	else
	{
		m_server.leave_trade(shared_from_this());
	}
}

void CTradeSession::handle_read_body(const boost::system::error_code& error)
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
			boost::bind(&CTradeSession::handle_read_header, shared_from_this(), boost::asio::placeholders::error));
	}
	else
	{
		m_server.leave_trade(shared_from_this());
	}
}

void CTradeSession::handle_write(const boost::system::error_code& error)
{
	if (!error)
	{
		// 在这里删除队列中发送成功的消息
		if (!m_trade_res_queue.empty())
		{
			m_trade_res_queue.pop_front();
		}

		// 判断是否还有发送的响应包
		if (!m_trade_res_queue.empty())
		{
			std::string &strMsg = m_trade_res_queue.front();
			boost::asio::async_write(m_socket,
				boost::asio::buffer(strMsg, strMsg.length()),
				boost::bind(&CTradeSession::handle_write, shared_from_this(),boost::asio::placeholders::error));
		}
	}
	else
	{
		m_server.leave_trade(shared_from_this());
	}
}

void CTradeSession::thread_handler_req(CTradeSession * pThis)
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

void CTradeSession::thread_handler_res(CTradeSession * pThis)
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

void CTradeSession::handle_req(std::string strPacket)
{
	Json::Value vRoot;
	if (m_reader.parse(strPacket, vRoot))
	{
		Json::Value vHead = vRoot["head"];
		std::string strTradeCode = vHead["s_trdcode"].asString();

		if (strTradeCode.compare(TS_FUNC_INSERT_ORDER) == 0)// 委托
		{
			handle_req_insert_order(vRoot);
		}
		else if (strTradeCode.compare(TS_FUNC_CANCEL_ORDER) == 0)// 撤单
		{
			handle_req_cancel_order(vRoot);
		}
		else if (strTradeCode.compare(TS_FUNC_ENTRUST_QUERY_FOR_ID) == 0)// id委托查询
		{
			handle_req_entrust_query_for_entrustID(vRoot);
		}
		else if (strTradeCode.compare(TS_FUNC_ENTRUST_QUERY) == 0)// 委托查询
		{
			handle_req_entrust_query(vRoot);
		}
		else if (strTradeCode.compare(TS_FUNC_TRADE_QUERY_FOR_ID) == 0)// id交易查询
		{
			handle_req_trade_query_for_tradeID(vRoot);
		}
		else if (strTradeCode.compare(TS_FUNC_TRADE_QUERY) == 0)// 交易查询
		{
			handle_req_trade_query(vRoot);
		}
		else if (strTradeCode.compare(TS_FUNC_HOLD_QUERY) == 0)// 持仓查询
		{
			handle_req_hold_query(vRoot);
		}
		else if (strTradeCode.compare(TS_FUNC_ASSET_DETAIL) == 0)// 资产查询
		{
			handle_req_asset_detail(vRoot);
		}
		else if (strTradeCode.compare(TS_FUNC_QUOTE_SUB) == 0)// 行情订阅（交易系统发送了行情订阅）
		{
		}
		else if (strTradeCode.compare(TS_FUNC_CANCEL_QUOTE_SUB) == 0)// 取消行情订阅
		{
		}
	}
	else// json串解析出错
	{
		//
	}
}

void CTradeSession::handle_req_insert_order(Json::Value & v)
{
	Json::Value vHead = v["head"];
	std::string strPacketID = vHead["s_trdid"].asString();

	Json::Value vBodyArray = v["body"];
	Json::Value vBody = vBodyArray[0];
	
	std::string strTicker     = vBody["ticker"].asString();
	int         nMarket       = vBody["market"].asInt();
	double      dbPrice       = vBody["price"].asDouble();
	double      dbStopPrice   = vBody["stop_price"].asDouble();
	long        nQuantity     = vBody["quantity"].asLargestUInt();
	int         nPriceType    = vBody["price_type"].asInt();
	int         nSide         = vBody["side"].asInt();
	int         nBusinessType = vBody["business_type"].asInt();

	XTPOrderInsertInfo order;
	strncpy(order.ticker, strTicker.c_str(), XTP_TICKER_LEN);
	order.order_client_id = 1;
	order.market          = (XTP_MARKET_TYPE)nMarket;
	order.price           = dbPrice;
	order.stop_price      = dbStopPrice;
	order.quantity        = nQuantity;
	order.price_type      = (XTP_PRICE_TYPE)nPriceType;
	order.side            = (XTP_SIDE_TYPE)nSide;
	order.business_type   = (XTP_BUSINESS_TYPE)nBusinessType;

	if (m_tradeReq.isConnected())// 如果和xtp服务器连接上了,那么就可以进行委托下单
	{
		if (!m_tradeReq.doEntrustOrder(&order, strPacketID))
		{
			send_error_res_packet(strPacketID, TS_FUNC_INSERT_ORDER, TS_RES_CODE_ENTRUST_FAILED, "委托失败");
		}
	}
	else// 和xtp交易服务器断开了，那么通知交易系统，和xtp连接断开，下单失败
	{

	}
}

void CTradeSession::handle_req_cancel_order(Json::Value & v)
{
	Json::Value vHead = v["head"];
	std::string strPacketID = vHead["s_trdid"].asString();

	Json::Value vBody = v["body"];
	uint64_t entrustID = vBody["order_xtp_id"].asUInt64();

	if (m_tradeReq.isConnected())// 如果和xtp服务器连接上了,
	{
		if (!m_tradeReq.doCancelEntrust(entrustID, strPacketID))
		{
			send_error_res_packet(strPacketID, TS_FUNC_CANCEL_ORDER, TS_RES_CODE_CANCEL_ENTRUST_FAILED, "撤单失败");
		}
	}
	else
	{
		//
	}
}

void CTradeSession::handle_req_entrust_query_for_entrustID(Json::Value & v)
{
	Json::Value vHead = v["head"];
	std::string strPacketID = vHead["s_trdid"].asString();

	Json::Value vBody = v["body"];

	uint64_t entrustID = vBody["order_xtp_id"].asUInt64();

	if (m_tradeReq.isConnected())// 如果和xtp服务器连接上了,
	{
		if (!m_tradeReq.doQueryEntrustForID(entrustID, strPacketID))
		{
			send_error_res_packet(strPacketID, TS_FUNC_ENTRUST_QUERY_FOR_ID, TS_RES_CODE_QUERY_ENTRUST_ID_FAILED, "通过委托ID查询委托失败");
		}
	}
	else
	{

	}
}

void CTradeSession::handle_req_entrust_query(Json::Value & v)
{
	Json::Value vHead = v["head"];
	std::string strPacketID = vHead["s_trdid"].asString();

	Json::Value vBody = v["body"];
	std::string strTicker = vBody["ticker"].asString();
	int64_t beginTime = vBody["begin_time"].asInt64();
	int64_t endTime = vBody["end_time"].asInt64();

	XTPQueryOrderReq queryParam;
	strncpy(queryParam.ticker, strTicker.c_str(), XTP_TICKER_LEN);
	queryParam.begin_time = beginTime;
	queryParam.end_time = endTime;
	
	if (m_tradeReq.isConnected())// 如果和xtp服务器连接上了,
	{
		if (!m_tradeReq.doQueryEntrust(&queryParam, strPacketID))
		{
			send_error_res_packet(strPacketID, TS_FUNC_ENTRUST_QUERY, TS_RES_CODE_QUERY_ENTRUST_FAILED, "委托查询失败");
		}
	}
	else
	{

	}
}

void CTradeSession::handle_req_trade_query_for_tradeID(Json::Value & v)
{
	Json::Value vHead = v["head"];
	std::string strPacketID = vHead["s_trdid"].asString();

	Json::Value vBody = v["body"];
	uint64_t tradeID = vBody["order_xtp_id"].asUInt64();

	if (m_tradeReq.isConnected())// 如果和xtp服务器连接上了,
	{
		if (!m_tradeReq.doQueryTradeForID(tradeID, strPacketID))
		{
			send_error_res_packet(strPacketID, TS_FUNC_TRADE_QUERY_FOR_ID, TS_RES_CODE_QUERY_TRADE_ID_FAILED, "通过成交ID查询成交失败");
		}
	}
	else
	{

	}
}

void CTradeSession::handle_req_trade_query(Json::Value & v)
{
	Json::Value vHead = v["head"];
	std::string strPacketID = vHead["s_trdid"].asString();

	Json::Value vBody = v["body"];

	std::string strTicker = vBody["ticker"].asString();
	int64_t beginTime = vBody["begin_time"].asInt64();
	int64_t endTime = vBody["end_time"].asInt64();

	XTPQueryTraderReq queryParam;
	strncpy(queryParam.ticker, strTicker.c_str(), XTP_TICKER_LEN);
	queryParam.begin_time = beginTime;
	queryParam.end_time = endTime;

	if (m_tradeReq.isConnected())// 如果和xtp服务器连接上了,
	{
		if (!m_tradeReq.doQueryTrade(&queryParam, strPacketID))
		{
			send_error_res_packet(strPacketID, TS_FUNC_TRADE_QUERY, TS_RES_CODE_QUERY_TRADE_FAILED, "成交查询失败");
		}
	}
	else
	{

	}
}

void CTradeSession::handle_req_hold_query(Json::Value & v)
{
	Json::Value vHead = v["head"];
	std::string strPacketID = vHead["s_trdid"].asString();

	Json::Value vBody = v["body"];
	std::string strTicker = vBody["ticker"].asString();

	if (m_tradeReq.isConnected())// 如果和xtp服务器连接上了,
	{
		if (!m_tradeReq.doQueryHold(strTicker.c_str(), strPacketID))
		{
			send_error_res_packet(strPacketID, TS_FUNC_HOLD_QUERY, TS_RES_CODE_QUERY_HOLD_FAILED, "持仓查询失败");
		}
	}
	else
	{

	}
}

void CTradeSession::handle_req_asset_detail(Json::Value & v)
{
	Json::Value vHead = v["head"];
	std::string strPacketID = vHead["s_trdid"].asString();

	if (m_tradeReq.isConnected())// 如果和xtp服务器连接上了,
	{
		if (!m_tradeReq.doQueryAssert(strPacketID))
		{
			send_error_res_packet(strPacketID, TS_FUNC_ASSET_DETAIL, TS_RES_CODE_QUERY_ASSERT_FAILED, "持仓查询失败");
		}
	}
	else
	{

	}
}

void CTradeSession::combo_head(Json::Value &vHead, std::string strPacketID, std::string strTradeCode, std::string strResCode, std::string strMsg)
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

bool CTradeSession::pushReqPacket(std::string &strPacket)
{
	boost::mutex::scoped_lock lock(m_mutexReq);
	if (!strPacket.empty())
	{
		m_trade_req_queue.push_back(strPacket);
		return true;
	}

	return false;
}

bool CTradeSession::popReqPacket(std::string & strPacket)
{
	boost::mutex::scoped_lock lock(m_mutexReq);

	if (!m_trade_req_queue.empty())
	{
		std::string &strMsg = m_trade_req_queue.front();
		if (strMsg.length() > 0)
		{
			strPacket = strMsg;
			m_trade_req_queue.pop_front();

			return true;
		}

		return false;
	}


	return false;
}

bool CTradeSession::pushResPacket(std::string & strPacket)
{
	boost::mutex::scoped_lock lock(m_mutexRes);
	if (!strPacket.empty())
	{
		m_trade_res_queue.push_back(strPacket);
		return true;
	}

	return false;

}

bool CTradeSession::popResPacket(std::string & strPacket)
{
	boost::mutex::scoped_lock lock(m_mutexRes);

	if (!m_trade_res_queue.empty())
	{
		std::string &strMsg = m_trade_res_queue.front();
		if (strMsg.length() > 0)
		{
			strPacket = strMsg;
			m_trade_res_queue.pop_front();

			return true;
		}

		return false;
	}


	return false;
}

void CTradeSession::triggerReqSignal()
{
	m_reqSema.signal();
}

void CTradeSession::triggerResSignal()
{
	m_resSema.signal();
}

void CTradeSession::send_error_res_packet(std::string strPacketID, std::string strTradeCode, std::string strResCode, std::string strMsg)
{
	Json::Value vHeadRes;
	combo_head(vHeadRes, strPacketID, strTradeCode, strResCode, strMsg);

	Json::Value vRes;
	vRes["head"] = vHeadRes;

	Json::FastWriter writer;
	std::string &strJson = writer.write(vRes);

	send(strJson);
}

uint64_t CTradeSession::getSessionID()
{
	return m_nSessionID;
}

void CTradeSession::start()
{
	static int nSessionID = 0;
	++nSessionID;
	m_nSessionID = nSessionID;

	// 连接xtp交易服务器
	{
		T_XtpTradeLogin xtpTradeServer;
		theApp.config.getXtpTradeInfo(xtpTradeServer, 0);

		T_XtpInitInfo xtpInitInfo;
		int nClientID;
		theApp.config.getTradeInit(xtpInitInfo, nClientID, 0);
		m_tradeReq.init(xtpInitInfo.szKey, xtpInitInfo.szCatalog, nClientID, m_pTradeSpi);

		if (!m_tradeReq.doLogin(xtpTradeServer.szUserName, xtpTradeServer.szPwd, xtpTradeServer.tAddr.szIP, xtpTradeServer.tAddr.nPort))
		{
			printf("session：%d, xtp交易服务器连接失败!\n", m_nSessionID);
		}
	}

	// 创建监听线程
	for (int i = 0; i < theApp.config.m_nThreadCount; ++i)
	{
		boost::thread thReq(boost::bind(CTradeSession::thread_handler_req, this));
		boost::thread thRes(boost::bind(CTradeSession::thread_handler_res, this));
	}

	// 将当前会话实例添加到队列中
	m_server.join_trade(shared_from_this());

	boost::asio::async_read(m_socket,
		boost::asio::buffer(m_szBuf, HEADER_LENGTH),
		boost::bind(&CTradeSession::handle_read_header, shared_from_this(), boost::asio::placeholders::error));
}

void CTradeSession::send(const std::string& msg)
{
	bool write_in_progress = !m_trade_res_queue.empty();
	m_trade_res_queue.push_back(msg);
	if (!write_in_progress)
	{
		std::string &strMsg = m_trade_res_queue.front();

		boost::asio::async_write(m_socket,
			boost::asio::buffer(strMsg, strMsg.length()),
			boost::bind(&CTradeSession::handle_write, shared_from_this(), boost::asio::placeholders::error));
	}
}

boost::asio::ip::tcp::socket& CTradeSession::get_socket()
{
	return m_socket;
}
