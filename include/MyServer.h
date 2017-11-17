#ifndef XHT_SERVER_H_
#define XHT_SERVER_H_
#include <set>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include "commonType.h"


#define TRADE_SERVER "交易服务"
#define QUOTE_SERVER "行情服务"

class CTradeSession;
class CQuoteSession;

typedef boost::shared_ptr<CTradeSession> TRADE_SESSION_PTR;
typedef boost::shared_ptr<CQuoteSession> QUOTE_SESSION_PTR;


class CMyServer
{
private:
	boost::asio::io_service& m_ioService;
	boost::asio::ip::tcp::acceptor m_acceptor;
	std::set<TRADE_SESSION_PTR> m_setTradeSession;		// 存储所有交易会话实例
	std::set<QUOTE_SESSION_PTR> m_setQuoteSession;		// 存储所有行情会话实例
	std::string m_strServerName;					    // 服务器名称


public:
	CMyServer(boost::asio::io_service& io_service,const boost::asio::ip::tcp::endpoint& endpoint, std::string strServerName);
	~CMyServer();

	std::string getServerName();

	void start_accept();
	void handle_accept_for_trade(TRADE_SESSION_PTR session, const boost::system::error_code& error);
	void handle_accept_for_quote(QUOTE_SESSION_PTR session, const boost::system::error_code& error);


	void join_trade(TRADE_SESSION_PTR participant);// 添加会话实例
	void leave_trade(TRADE_SESSION_PTR participant);// 删除会话实例

	void join_quote(QUOTE_SESSION_PTR participant);// 添加会话实例
	void leave_quote(QUOTE_SESSION_PTR participant);// 删除会话实例




};

typedef boost::shared_ptr<CMyServer> server_ptr;


#endif