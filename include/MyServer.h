#ifndef XHT_SERVER_H_
#define XHT_SERVER_H_
#include <set>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include "commonType.h"


#define TRADE_SERVER "���׷���"
#define QUOTE_SERVER "�������"

class CTradeSession;
class CQuoteSession;

typedef boost::shared_ptr<CTradeSession> TRADE_SESSION_PTR;
typedef boost::shared_ptr<CQuoteSession> QUOTE_SESSION_PTR;


class CMyServer
{
private:
	boost::asio::io_service& m_ioService;
	boost::asio::ip::tcp::acceptor m_acceptor;
	std::set<TRADE_SESSION_PTR> m_setTradeSession;		// �洢���н��׻Ựʵ��
	std::set<QUOTE_SESSION_PTR> m_setQuoteSession;		// �洢��������Ựʵ��
	std::string m_strServerName;					    // ����������


public:
	CMyServer(boost::asio::io_service& io_service,const boost::asio::ip::tcp::endpoint& endpoint, std::string strServerName);
	~CMyServer();

	std::string getServerName();

	void start_accept();
	void handle_accept_for_trade(TRADE_SESSION_PTR session, const boost::system::error_code& error);
	void handle_accept_for_quote(QUOTE_SESSION_PTR session, const boost::system::error_code& error);


	void join_trade(TRADE_SESSION_PTR participant);// ��ӻỰʵ��
	void leave_trade(TRADE_SESSION_PTR participant);// ɾ���Ựʵ��

	void join_quote(QUOTE_SESSION_PTR participant);// ��ӻỰʵ��
	void leave_quote(QUOTE_SESSION_PTR participant);// ɾ���Ựʵ��




};

typedef boost::shared_ptr<CMyServer> server_ptr;


#endif