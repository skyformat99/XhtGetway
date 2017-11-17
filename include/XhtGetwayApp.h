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
	server_ptr     m_tradeServer;	// 交易服务器
	server_ptr     m_quoteServer;	// 行情服务器


	void setTradeServer(server_ptr tradeServer);
	void setQuoteServer(server_ptr quoteServer);

	

};

extern CXhtGetwayApp theApp;

#endif

