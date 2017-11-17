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

	// �ж϶��Ʊ����ʱ����ʽ�Ƿ���ȷ����ȷ�ĸ�ʽΪ���Զ��ŷָ���Ʊ���룩

	std::set<std::string> setSubTickersSZ;// ����Ҫ���ĵ����й�Ʊ����
	std::set<std::string> setSubTickersSH;// ����Ҫ���ĵĻ��й�Ʊ����


	// ��Ҫ���ĵĹ�Ʊ������зָ�
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

	/*****��Ҫ���ĵĹ�Ʊ���Ϊxtp����������ɽ��ܵ����ݸ�ʽ*****/

	// ����
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

	// ����
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

	// �жϣ����Ʊȡ������ʱ����ʽ�Ƿ���ȷ����ȷ�ĸ�ʽΪ���Զ��ŷָ���Ʊ���룬���ҹ�Ʊ������г���׺��


	std::set<std::string> setSubTickersSZ;// ����Ҫȡ�����ĵ����й�Ʊ����
	std::set<std::string> setSubTickersSH;// ����Ҫȡ�����ĵĻ��й�Ʊ����

	// ��Ҫ���ĵĹ�Ʊ������зָ�
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


	/*****��Ҫ���ĵĹ�Ʊ���Ϊxtp����������ɽ��ܵ����ݸ�ʽ*****/

	// ����
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

	// ����
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
