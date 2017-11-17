#include <vector>
#include <set>
#include <boost/tokenizer.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include "../include/XtpQuoteReq.h"
#include "../include/XhtGetwayApp.h"



CXtpQuoteReq::CXtpQuoteReq()
{
	m_pQuoteApi = NULL;
	m_uQuoteSession = 0;
}

CXtpQuoteReq::~CXtpQuoteReq()
{
}

void CXtpQuoteReq::init(int nClientID, const char *szQuoteCatalog, MyQuoteSpi *pQuoteSpi)
{
	m_pQuoteApi = XTP::API::QuoteApi::CreateQuoteApi(nClientID, szQuoteCatalog);
	
	m_pQuoteApi->RegisterSpi(pQuoteSpi);
}

uint64_t CXtpQuoteReq::doLogin(const char *szUserName, const char *szPwd, const char *szIP, int nPort)
{
	if (!m_pQuoteApi)
		return false;

	m_uQuoteSession = m_pQuoteApi->Login(szIP, nPort, szUserName, szPwd, XTP_PROTOCOL_TCP);
	if (0 == m_uQuoteSession)
	{
		return 0;
	}
	
	return m_uQuoteSession;
}

int CXtpQuoteReq::doLoginout()
{
	if (!m_pQuoteApi)
		return false;

	return m_pQuoteApi->Logout();
}

bool CXtpQuoteReq::doSubQuote(const char * szTicker)
{
	if (!m_pQuoteApi)
		return false;

	if (!szTicker || strlen(szTicker) == 0)
		return false;

	// 判断多股票订阅时，格式是否正确（正确的格式为：以逗号分隔股票代码）

	std::set<std::string> setSubTickersSZ;// 保存要订阅的深市股票代码
	std::set<std::string> setSubTickersSH;// 保存要订阅的沪市股票代码


	// 对要订阅的股票代码进行分隔
	std::vector<std::string> vecTicker;   
	boost::split(vecTicker, szTicker, boost::is_any_of(","));

	int nTickerCount = vecTicker.size();

	for (int i = 0; i < nTickerCount; i++)
	{
		std::string strTickerTemp = vecTicker.at(i);
		int pos = strTickerTemp.find(".");
		std::string strTicker = strTickerTemp.substr(0, pos);
		std::string strMarket = strTickerTemp.substr(pos + 1, -1);
		if (strMarket.compare("SZ") == 0)
		{
			T_SubTimers subTimers;
			subTimers.strTicker = strTicker;
			std::set<T_SubTimers>::iterator it = m_setSubTickersSZ.find(subTimers);
			if (it == m_setSubTickersSZ.end())
			{
				subTimers.nTimers = 1;
				m_setSubTickersSZ.insert(subTimers);
				setSubTickersSZ.insert(strTicker);
			}
			else
			{
				++subTimers.nTimers;
			}
		}
		else if (strMarket.compare("SH") == 0)
		{
			T_SubTimers subTimers;
			subTimers.strTicker = strTicker;
			std::set<T_SubTimers>::iterator it = m_setSubTickersSH.find(subTimers);
			if (it == m_setSubTickersSH.end())
			{
				subTimers.nTimers = 1;
				m_setSubTickersSH.insert(subTimers);
				setSubTickersSH.insert(strTicker);
			}
			else
			{
				++subTimers.nTimers;
			}
		}
	}

	/*****将要订阅的股票组合为xtp行情服务器可接受的数据格式*****/

	// 深市
	if (!setSubTickersSZ.empty())
	{
		int nCount = setSubTickersSZ.size();
		char** szTickerArray = new char*[nCount];
		std::set<std::string>::iterator it;
		int i = 0;
		for (it = setSubTickersSZ.begin(); it != setSubTickersSZ.end(); ++it)
		{
			szTickerArray[i++] = new char[7];
			std::string strTicker = *it;

			strcpy(szTickerArray[i], strTicker.c_str());
		}

		if (0 != m_pQuoteApi->SubscribeMarketData(szTickerArray, nTickerCount, XTP_EXCHANGE_SZ))
		{
			return false;
		}
	}

	// 沪市
	if (!setSubTickersSH.empty())
	{
		int nCount = setSubTickersSH.size();
		char** szTickerArray = new char*[nCount];
		std::set<std::string>::iterator it;
		int i = 0;
		for (it = setSubTickersSH.begin(); it != setSubTickersSH.end(); ++it)
		{
			szTickerArray[i++] = new char[7];
			std::string strTicker = *it;

			strcpy(szTickerArray[i], strTicker.c_str());
		}

		if (0 != m_pQuoteApi->SubscribeMarketData(szTickerArray, nTickerCount, XTP_EXCHANGE_SH))
		{
			return false;
		}
	}

	return true;
}

int CXtpQuoteReq::doUnSubQuote(const char * szTicker)
{
	if (!m_pQuoteApi)
		return -1;

	if (!szTicker || strlen(szTicker) == 0)
		return -1;

	// 判断：多股票取消订阅时，格式是否正确（正确的格式为：以逗号分隔股票代码，并且股票代码带市场后缀）


	std::set<std::string> setSubTickersSZ;// 保存要取消订阅的深市股票代码
	std::set<std::string> setSubTickersSH;// 保存要取消订阅的沪市股票代码

	// 对要订阅的股票代码进行分隔
	std::vector<std::string> vecTicker;
	boost::split(vecTicker, szTicker, boost::is_any_of(","));

	int nTickerCount = vecTicker.size();

	for (int i = 0; i < nTickerCount; i++)
	{
		std::string strTickerTemp = vecTicker.at(i);
		int pos = strTickerTemp.find(".");
		std::string strTicker = strTickerTemp.substr(0, pos);
		std::string strMarket = strTickerTemp.substr(pos + 1, -1);
		if (strMarket.compare("SZ") == 0)
		{
			setSubTickersSZ.insert(strTicker);
		}
		else if (strMarket.compare("SH") == 0)
		{
			setSubTickersSH.insert(strTicker);
		}
	}

	int nRet;


	/*****将要订阅的股票组合为xtp行情服务器可接受的数据格式*****/

	// 深市
	if (!setSubTickersSZ.empty())
	{
		int nCount = setSubTickersSZ.size();
		char** szTickerArray = new char*[nCount];
		std::set<std::string>::iterator it;
		int i = 0;
		for (it = setSubTickersSZ.begin(); it != setSubTickersSZ.end(); ++it)
		{
			szTickerArray[i++] = new char[7];
			std::string strTicker = *it;

			strcpy(szTickerArray[i], strTicker.c_str());
		}

		if (0 != m_pQuoteApi->UnSubscribeMarketData(szTickerArray, nTickerCount, XTP_EXCHANGE_SZ))
		{
			return false;
		}
	}

	// 沪市
	if (!setSubTickersSH.empty())
	{
		int nCount = setSubTickersSH.size();
		char** szTickerArray = new char*[nCount];
		std::set<std::string>::iterator it;
		int i = 0;
		for (it = setSubTickersSH.begin(); it != setSubTickersSH.end(); ++it)
		{
			szTickerArray[i++] = new char[7];
			std::string strTicker = *it;

			strcpy(szTickerArray[i], strTicker.c_str());
		}

		if (0 != m_pQuoteApi->UnSubscribeMarketData(szTickerArray, nTickerCount, XTP_EXCHANGE_SH))
		{
			return false;
		}
	}

	return true;
}
