#include <string>
#include "../include/XtpTradeReq.h"


const char* const g_Msg[] = 
{
	"操作成功"
	"xtp交易初始化失败",
	"xtp登录失败",
};


CXtpTradeReq::CXtpTradeReq()
{
	m_bLogined = false;
	memset(m_szErrorMsg, 0, MAX_ERROR_MSG);
	m_pXtpTradeReq = NULL;
	m_uSessionID = 0;
}

CXtpTradeReq::~CXtpTradeReq()
{
}

bool CXtpTradeReq::setLastErrorMsgV1(int nPos)
{
	// 检查下标是否合法
	if (nPos < 0 || nPos > TG_LAST_END - 1)
		return false;

	// 检查缓冲区是否能存储预定义错误信息的长度
	if (strlen(g_Msg[nPos]) > MAX_ERROR_MSG)
		return false;

	int nLen = strlen(g_Msg[nPos]);
	strncpy(m_szErrorMsg, g_Msg[nPos], nLen);
	m_szErrorMsg[nLen] = '\0';

	
	return true;
}

bool CXtpTradeReq::setLastErrorMsgV2(const char * szErrorMsg)
{
	return false;
}

bool CXtpTradeReq::createCatalog(const char * szCatalog)
{
	if (!szCatalog)
		return false;

	if (strlen(szCatalog) == 0)
		return 0;

	return false;
}

long CXtpTradeReq::getRequestID()
{
	static long nRequestID = 0;
	return ++nRequestID;
}

bool CXtpTradeReq::init(const char *szKey, const char *szFilePath, int nClientID, CXtpTraderRes *pTradeSpi)
{
	if (!szFilePath || !szKey)
	{
		setLastErrorMsgV1(TG_XTP_INIT_FAILED);
		return false;
	}

	if (strlen(szFilePath) == 0 || strlen(szKey) == 0)
	{
		setLastErrorMsgV1(TG_XTP_INIT_FAILED);
		return false;
	}

	m_pXtpTradeReq = XTP::API::TraderApi::CreateTraderApi(nClientID, szFilePath);

	if (!m_pXtpTradeReq)
	{
		setLastErrorMsgV1(TG_XTP_INIT_FAILED);
		return false;
	}

	m_pXtpTradeReq->SetSoftwareVersion("1.1.0");
	m_pXtpTradeReq->SetSoftwareKey(szKey);

	m_pXtpTradeReq->RegisterSpi(pTradeSpi);// 注册回调对象

	setLastErrorMsgV1(TG_SUCCESS);

	return true;
}

bool CXtpTradeReq::doLogin(const char *szUserName, const char *szPwd, const char *szIP, int nPort)
{
	if (nPort <= 0)
	{
		setLastErrorMsgV1(TG_XTP_LOGIN_FAILED);
		return false;
	}

	if (!szUserName || !szPwd || !szIP)
	{
		setLastErrorMsgV1(TG_XTP_LOGIN_FAILED);
		return false;
	}

	if (strlen(szUserName) == 0 ||
		strlen(szPwd) == 0 ||
		strlen(szIP) == 0)
	{
		setLastErrorMsgV1(TG_XTP_LOGIN_FAILED);
		return false;
	}

	if (!m_pXtpTradeReq)
	{
		return false;
	}

	m_uSessionID = m_pXtpTradeReq->Login(szIP, nPort, szUserName, szPwd, XTP_PROTOCOL_TCP);
	if (0 == m_uSessionID)
	{
		XTPRI *p = m_pXtpTradeReq->GetApiLastError();
		return false;
	}

	setConnectStatus(true);

	return true;
}

int CXtpTradeReq::loginout()
{
	if (!m_pXtpTradeReq->Logout(m_uSessionID))
		return -1;

	setConnectStatus(false);
	
	return 0;
}

bool CXtpTradeReq::doEntrustOrder(XTPOrderInsertInfo *order, std::string strPacketID)
{
	if (!m_pXtpTradeReq)
		return false;

	uint64_t orderID = m_pXtpTradeReq->InsertOrder(order, m_uSessionID);
	if (!orderID)
		return false;

	addPacketID(orderID, strPacketID);

	return true;
}

bool CXtpTradeReq::doCancelEntrust(const uint64_t entrustID, std::string strPacketID)
{
	if (!m_pXtpTradeReq)
		return false;

	uint64_t cancelOrderID = m_pXtpTradeReq->CancelOrder(entrustID, m_uSessionID);
	if (!cancelOrderID)
		return false;

	addPacketID(cancelOrderID, strPacketID);

	return true;
}

bool CXtpTradeReq::doQueryEntrust(const XTPQueryOrderReq *queryParam, std::string strPacketID)
{
	if (!m_pXtpTradeReq)
		return false;

	int nRequestID = getRequestID();

	int nRet = m_pXtpTradeReq->QueryOrders(queryParam, m_uSessionID, nRequestID);
	if (0 != nRet)
		return false;

	addPacketID(nRequestID, strPacketID);

	return true;
}

bool CXtpTradeReq::doQueryEntrustForID(const uint64_t entrustID, std::string strPacketID)
{
	if (!m_pXtpTradeReq)
		return false;

	int nRequestID = getRequestID();

	int nRet = m_pXtpTradeReq->QueryOrderByXTPID(entrustID, m_uSessionID, nRequestID);
	if (0 != nRet)
		return false;

	addPacketID(nRequestID, strPacketID);

	return true;
}

bool CXtpTradeReq::doQueryTrade(XTPQueryTraderReq *queryParam, std::string strPacketID)
{
	if (!m_pXtpTradeReq)
		return false;

	int nRequestID = getRequestID();

	int nRet = m_pXtpTradeReq->QueryTrades(queryParam, m_uSessionID, nRequestID);
	if (0 != nRet)
		return false;

	addPacketID(nRequestID, strPacketID);

	return true;
}

bool CXtpTradeReq::doQueryTradeForID(const uint64_t tradeID, std::string strPacketID)
{
	if (!m_pXtpTradeReq)
		return false;

	int nRequestID = getRequestID();

	int nRet = m_pXtpTradeReq->QueryTradesByXTPID(tradeID, m_uSessionID, nRequestID);
	if (0 != nRet)
		return false;

	addPacketID(nRequestID, strPacketID);

	return true;
}

bool CXtpTradeReq::doQueryHold(const char *ticker, std::string strPacketID)
{
	if (!m_pXtpTradeReq)
		return false;

	int nRequestID = getRequestID();

	int nRet = m_pXtpTradeReq->QueryPosition(ticker, m_uSessionID, nRequestID);
	if (0 != nRet)
		return false;

	addPacketID(nRequestID, strPacketID);

	return true;
}

bool CXtpTradeReq::doQueryAssert(std::string strPacketID)
{
	if (!m_pXtpTradeReq)
		return false;

	int nRequestID = getRequestID();
	if (0 != m_pXtpTradeReq->QueryAsset(m_uSessionID, nRequestID))
		return false;

	addPacketID(nRequestID, strPacketID);

	return true;
}

char * CXtpTradeReq::getLastErrorMsg()
{
	return m_szErrorMsg;
}

void CXtpTradeReq::setConnectStatus(bool bConnected)
{
	m_bLogined = bConnected;
}

bool CXtpTradeReq::isConnected()
{
	return m_bLogined;
}

std::string CXtpTradeReq::getPacketID(uint64_t key)
{
	std::string strPacket;
	std::map<uint64_t, std::string>::iterator it = m_mapPacketID.find(key);
	if (it != m_mapPacketID.end())
	{
		strPacket = it->second;
	}

	return strPacket;
}

void CXtpTradeReq::addPacketID(uint64_t key, std::string strPacketID)
{
	m_mapPacketID.insert(std::make_pair(key, strPacketID));
}

void CXtpTradeReq::deletePacketID(uint64_t key)
{
	m_mapPacketID.erase(key);
}