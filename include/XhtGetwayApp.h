#ifndef XHT_GETWAY_APP_H_
#define XHT_GETWAY_APP_H_
#include <string>
#include <map>
#include "MyServer.h"
#include "XtpTradeReq.h"
#include "XtpQuoteReq.h"
#include "../include/Config.h"



class CXhtGetwayApp
{
public:
	CXhtGetwayApp();
	~CXhtGetwayApp();

public:
	CConfig config;
	server_ptr     m_tradeServer;	// ���׷�����
	server_ptr     m_quoteServer;	// ���������


	void setTradeServer(server_ptr tradeServer);
	void setQuoteServer(server_ptr quoteServer);

	

};

extern CXhtGetwayApp theApp;

#endif

