#include <string>
#include <io.h>
#include <direct.h>
#include "../include/Config.h"
#include <commonType.h>




CConfig::CConfig()
{
	m_nTradeServerCount = 0; 
	m_nQuoteServerCount = 0;
	m_nThreadCount = 1;
}


CConfig::~CConfig()
{
}

bool CConfig::readConfig()
{
	SI_Error rc = m_ini.LoadFile(X("XhtGetway.ini"));
	if (rc < 0)
		return false;

	m_nThreadCount = m_ini.GetLongValue(X("server"), X("threadCount"), 1);

	const char *szIP = m_ini.GetValue(X("server"), X("ip"));
	int nPortTrade = m_ini.GetLongValue(X("server"), X("portTrade"));
	int nPortQuote = m_ini.GetLongValue(X("server"), X("portQuote"));

	const char *szClientID = m_ini.GetValue(X("server"), X("clientID"), X("1"));
	

	int nLen = strlen(szIP);

	if (!szIP || nLen == 0)
		return false;

	strncpy(m_tTradeGetway.szIP, szIP, nLen);
	m_tTradeGetway.szIP[nLen] = '\0';
	m_tTradeGetway.nPort = nPortTrade;

	strncpy(m_tQuoteGetway.szIP, szIP, nLen);
	m_tQuoteGetway.szIP[nLen] = '\0';
	m_tQuoteGetway.nPort = nPortQuote;


	const char *szTradeCatalog1 = m_ini.GetValue(X("xtpTrade"), X("catalog"), X("d:\\xtpGetway\\trade1\\"));
	const char *szIP1 = m_ini.GetValue(X("xtpTrade"), X("ip1"));
	int nPort1 = m_ini.GetLongValue(X("xtpTrade"), X("port1"));
	const char *szUserName1 = m_ini.GetValue(X("xtpTrade"), X("username1"));
	const char *szPwd1 = m_ini.GetValue(X("xtpTrade"), X("pwd1"));
	const char *szKey1 = m_ini.GetValue(X("xtpTrade"), X("key1"));

	if (nPort1 > 0 && 
		szIP1 && strlen(szIP1) > 0 &&
		szUserName1 && strlen(szUserName1) > 0 &&
		szPwd1 && strlen(szPwd1) > 0 &&
		szKey1 && strlen(szKey1) > 0)
	{
		m_tXTPTradeServer[0].tAddr.nPort = nPort1;
		strncpy(m_tXTPTradeServer[0].tAddr.szIP, szIP1, strlen(szIP1));
		strncpy(m_tXTPTradeServer[0].szUserName, szUserName1, strlen(szUserName1));
		strncpy(m_tXTPTradeServer[0].szPwd, szPwd1, strlen(szPwd1));
		strncpy(m_xtpTradeInit[0].szKey, szKey1, strlen(szKey1));
		strncpy(m_xtpTradeInit[0].szCatalog, szTradeCatalog1, MAX_CATALOG);

		mkdir(szTradeCatalog1);

		++m_nTradeServerCount;
	}
	else
	{
		return false;
	}

	const char *szTradeCatalog2 = m_ini.GetValue(X("xtpTrade"), X("catalog"), X("d:\\xtpGetway\\trade2\\"));
	const char *szIP2 = m_ini.GetValue(X("xtpTrade"), X("ip2"));
	int nPort2 = m_ini.GetLongValue(X("xtpTrade"), X("port2"));
	const char *szUserName2 = m_ini.GetValue(X("xtpTrade"), X("username2"));
	const char *szPwd2 = m_ini.GetValue(X("xtpTrade"), X("pwd2"));
	const char *szKey2 = m_ini.GetValue(X("xtpTrade"), X("key2"));

	if (nPort2 > 0 &&
		szIP2 && strlen(szIP2) > 0 &&
		szUserName2 && strlen(szUserName2) > 0 &&
		szPwd2 && strlen(szPwd2) > 0 &&
		szKey2 && strlen(szKey2) > 0)
	{
		m_tXTPTradeServer[1].tAddr.nPort = nPort2;
		strncpy(m_tXTPTradeServer[1].tAddr.szIP, szIP2, strlen(szIP2));
		strncpy(m_tXTPTradeServer[1].szUserName, szUserName2, strlen(szUserName2));
		strncpy(m_tXTPTradeServer[1].szPwd, szPwd2, strlen(szPwd2));
		strncpy(m_xtpTradeInit[1].szKey, szKey2, strlen(szKey2));
		strncpy(m_xtpTradeInit[1].szCatalog, szTradeCatalog2, MAX_CATALOG);

		mkdir(szTradeCatalog2);
		++m_nTradeServerCount;
	}

	const char *szQuoteCatalog1 = m_ini.GetValue(X("xtpQuote"), X("catalog"), X("d:\\xtpGetway\\quote1\\"));
	

	const char *szQuoteIP1 = m_ini.GetValue(X("xtpQuote"), X("ip1"));
	int nQuotePort1 = m_ini.GetLongValue(X("xtpQuote"), X("port1"));
	const char *szQuoteUserName1 = m_ini.GetValue(X("xtpQuote"), X("username1"));
	const char *szQuotePwd1 = m_ini.GetValue(X("xtpQuote"), X("pwd1"));

	if (nQuotePort1 > 0 &&
		szQuoteIP1 && strlen(szQuoteIP1) > 0 &&
		szQuoteUserName1 && strlen(szQuoteUserName1) > 0 &&
		szQuotePwd1 && strlen(szQuotePwd1) > 0)
	{
		m_tXTPQuoteServer[1].tAddr.nPort = nQuotePort1;
		strncpy(m_tXTPQuoteServer[0].tAddr.szIP, szQuoteIP1, strlen(szQuoteIP1));
		strncpy(m_tXTPQuoteServer[0].szUserName, szQuoteUserName1, strlen(szQuoteUserName1));
		strncpy(m_tXTPQuoteServer[0].szPwd, szQuotePwd1, strlen(szQuotePwd1));
		strncpy(m_xtpQuoteInit[0].szCatalog, szQuoteCatalog1, MAX_CATALOG);
		mkdir(szQuoteCatalog1);

		++m_nQuoteServerCount;
	}

	const char *szQuoteCatalog2 = m_ini.GetValue(X("xtpQuote"), X("catalog"), X("d:\\xtpGetway\\quote2\\"));
	const char *szQuoteIP2 = m_ini.GetValue(X("xtpQuote"), X("ip2"));
	int nQuotePort2 = m_ini.GetLongValue(X("xtpQuote"), X("port2"));
	const char *szQuoteUserName2 = m_ini.GetValue(X("xtpQuote"), X("username2"));
	const char *szQuotePwd2 = m_ini.GetValue(X("xtpQuote"), X("pwd2"));

	if (nQuotePort2 > 0 &&
		szQuoteIP2 && strlen(szQuoteIP2) > 0 &&
		szQuoteUserName2 && strlen(szQuoteUserName2) > 0 &&
		szQuotePwd2 && strlen(szQuotePwd2) > 0)
	{
		m_tXTPQuoteServer[1].tAddr.nPort = nQuotePort2;
		strncpy(m_tXTPQuoteServer[1].tAddr.szIP, szQuoteIP2, strlen(szQuoteIP2));
		strncpy(m_tXTPQuoteServer[1].szUserName, szQuoteUserName2, strlen(szQuoteUserName2));
		strncpy(m_tXTPQuoteServer[1].szPwd, szQuotePwd2, strlen(szQuotePwd2));
		strncpy(m_xtpQuoteInit[1].szCatalog, szQuoteCatalog2, MAX_CATALOG);
		mkdir(szQuoteCatalog2);

		++m_nQuoteServerCount;
	}

	// 确保行情和交易用到的目录都存在
	
	

	return true;
}

void CConfig::getTradeGewayAddr(char * szServerIP, int nLen, int & nPort)
{
	if (nLen > MAX_IP)
		return;

	strncpy(szServerIP, m_tTradeGetway.szIP, MAX_IP);
	nPort = m_tTradeGetway.nPort;
}

void CConfig::getQuoteGewayAddr(char * szServerIP, int nLen, int & nPort, bool bWithIP/* = false*/)
{
	if (nLen > MAX_IP)
		return;

	if (bWithIP)
	{
		strncpy(szServerIP, m_tQuoteGetway.szIP, MAX_IP);
	}

	nPort = m_tQuoteGetway.nPort;
}

void CConfig::getTradeInit(T_XtpInitInfo & xtpTradeInit, int & nClientID, int pos)
{
	if (pos > m_nTradeServerCount - 1)
		return;

	xtpTradeInit = m_xtpTradeInit[pos];
	nClientID = getTradeClientID();
}

void CConfig::getQuoteInit(T_XtpInitInfo & xtpQuoteInit, int & nClientID, int pos)
{
	if (pos > m_nQuoteServerCount - 1)
		return;

	xtpQuoteInit = m_xtpQuoteInit[pos];
	nClientID = getQuoteClientID();
}

int CConfig::getTradeClientID()
{
	static int nTradeClientID = 0;
	return ++nTradeClientID;
}

int CConfig::getQuoteClientID()
{
	static int nQuoteClientID = 0;
	return ++nQuoteClientID;
}

bool CConfig::getXtpTradeInfo(T_XtpTradeLogin &xtpTradeInfo, int pos)
{
	if (pos >= m_nTradeServerCount)
		return false;

	xtpTradeInfo = m_tXTPTradeServer[pos];

	return true;
}

bool CConfig::getXtpQuoteInfo(T_XtpQuoteLogin & xtpQuoteInfo, int pos)
{
	if (pos >= m_nQuoteServerCount)
		return false;

	xtpQuoteInfo = m_tXTPQuoteServer[pos];

	return true;
}

void CConfig::mkdirs(const char *muldir)
{
	char    str[512];
	strncpy(str, muldir, 512);

	int len = strlen(str);

	for (int i = 0; i<len; i++)
	{
		if (str[i] == '\\')
		{
			char szCata[256] = { 0 };
			strncpy(szCata, str, i);

			if (_access(szCata, 0) != 0)
			{
				_mkdir(szCata);
			}
		}
	}
}

