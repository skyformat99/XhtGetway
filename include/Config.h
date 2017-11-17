#ifndef XHT_TG_CONFIG_H_
#define XHT_TG_CONFIG_H_
#include <set>
#include <string>
#include <SimpleIni.h>
#include <commonType.h>

#define MAX_IP 16
#define MAX_API_TYPE 64
#define MAX_CATALOG 256
#define MAX_USERNAME 64
#define MAX_PWD 64
#define MAX_KEY 64

typedef struct _XtpInitInfo
{
	char szKey[MAX_KEY];
	char szCatalog[MAX_CATALOG];
	_XtpInitInfo()
	{
		memset(this, 0, sizeof(_XtpInitInfo));
	}
}T_XtpInitInfo;

typedef struct _Addr
{
	char szIP[MAX_IP];
	int nPort;
	_Addr()
	{
		memset(this, 0, sizeof(_Addr));
	}
}T_Addr;

typedef struct _XtpTradeLogin
{
	T_Addr tAddr;
	char szUserName[MAX_USERNAME];
	char szPwd[MAX_PWD];
	_XtpTradeLogin()
	{
		memset(this, 0, sizeof(_XtpTradeLogin));
	}
}T_XtpTradeLogin;

typedef struct _XtpQuoteLogin
{
	T_Addr tAddr;
	char szUserName[MAX_USERNAME];
	char szPwd[MAX_PWD];
	_XtpQuoteLogin()
	{
		memset(this, 0, sizeof(_XtpQuoteLogin));
	}
}T_XtpQuoteLogin;

class CConfig
{
public:
	CConfig();
	~CConfig();

	bool readConfig();

	/*
	 * @name getTradeGewayAddr
	 *
	 * @brief ��ȡ����ǰ�õ�ַ
	 *
	 * @param szServerIP ���ڴ洢ip�Ļ�����
	 * @param nLen       szServerIP�Ĵ�С
	 * @param nPort      ���ڴ洢�˿�
	 * @param nPos       ��ȡ��һ��IP
	 */
	void getTradeGewayAddr(char *szServerIP, int nLen, int &nPor);

	/*
	* @name getQuoteGewayAddr
	*
	* @brief ��ȡ����ǰ�õ�ַ
	*
	* @param szServerIP ���ڴ洢ip�Ļ�����
	* @param nLen       szServerIP�Ĵ�С
	* @param nPort      ���ڴ洢�˿�
	* @param bWithIP    �Ƿ񸽴�IP
	*/
	void getQuoteGewayAddr(char *szServerIP, int nLen, int &nPort, bool bWithIP = false);

	void getTradeInit(T_XtpInitInfo &xtpTradeInit, int &nClientID, int pos);

	void getQuoteInit(T_XtpInitInfo &xtpQuoteInit, int &nClientID, int pos);

	int getTradeClientID();

	int getQuoteClientID();

	bool getXtpTradeInfo(T_XtpTradeLogin &xtpTradeInfo, int pos);

	bool getXtpQuoteInfo(T_XtpQuoteLogin &xtpQuoteInfo, int pos);

	// �����༶Ŀ¼
	void mkdirs(const char *muldir);

public:
	CSimpleIni m_ini;

	T_XtpInitInfo m_xtpTradeInit[2];
	T_XtpInitInfo m_xtpQuoteInit[2];

	T_Addr m_tTradeGetway;// ����ǰ�õĵ�ַ
	T_Addr m_tQuoteGetway;// ����ǰ�õĵ�ַ

	T_XtpTradeLogin m_tXTPTradeServer[2];// xtp���׷�������ַ
	T_XtpQuoteLogin m_tXTPQuoteServer[2];// xtp�����������ַ

	int m_nThreadCount;


	int m_nTradeServerCount;
	int m_nQuoteServerCount;


};

#endif
