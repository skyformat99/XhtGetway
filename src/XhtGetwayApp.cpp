#include "../include/XhtGetwayApp.h"
#include "../include/QuoteSession.h"

CXhtGetwayApp theApp;

CXhtGetwayApp::CXhtGetwayApp()
	:m_tradeServer(NULL),
	m_quoteServer(NULL)
{
}


CXhtGetwayApp::~CXhtGetwayApp()
{
}

void CXhtGetwayApp::setTradeServer(server_ptr tradeServer)
{
	m_tradeServer = tradeServer;
}

void CXhtGetwayApp::setQuoteServer(server_ptr quoteServer)
{
	m_quoteServer = quoteServer;
}
