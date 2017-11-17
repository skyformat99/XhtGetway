#include <algorithm>
#include "../include/MyServer.h"
#include "../include/TradeSession.h"
#include "../include/QuoteSession.h"


CMyServer::CMyServer(boost::asio::io_service& io_service, const boost::asio::ip::tcp::endpoint& endpoint, std::string strServerName)
	: m_ioService(io_service),
	m_acceptor(io_service, endpoint),
	m_strServerName(strServerName)
{
	start_accept();
}

CMyServer::~CMyServer()
{
}

std::string CMyServer::getServerName()
{
	return m_strServerName;
}

void CMyServer::start_accept()
{
	if (m_strServerName.compare(TRADE_SERVER) == 0)
	{
		TRADE_SESSION_PTR new_session(new CTradeSession(m_ioService, *this, true));
		m_acceptor.async_accept(new_session->get_socket(),
			boost::bind(&CMyServer::handle_accept_for_trade, this, new_session, boost::asio::placeholders::error));
	}
	else if (m_strServerName.compare(QUOTE_SERVER) == 0)
	{
		QUOTE_SESSION_PTR new_session(new CQuoteSession(m_ioService, *this));
		m_acceptor.async_accept(new_session->get_socket(),
			boost::bind(&CMyServer::handle_accept_for_quote, this, new_session, boost::asio::placeholders::error));
	}
}

void CMyServer::handle_accept_for_trade(TRADE_SESSION_PTR session, const boost::system::error_code& error)
{
	if (!error)
	{
		session->start();// 启动与客户端通信的会话实例
	}

	start_accept();
}

void CMyServer::handle_accept_for_quote(QUOTE_SESSION_PTR session, const boost::system::error_code & error)
{
	if (!error)
	{
		session->start();
	}

	start_accept();
}

void CMyServer::join_trade(TRADE_SESSION_PTR participant)
{
	m_setTradeSession.insert(participant);
}

void CMyServer::leave_trade(TRADE_SESSION_PTR participant)
{
}

void CMyServer::join_quote(QUOTE_SESSION_PTR participant)
{
	m_setQuoteSession.insert(participant);
}

void CMyServer::leave_quote(QUOTE_SESSION_PTR participant)
{

}

