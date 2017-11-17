#ifndef XHT_TRADE_SESSION_H_
#define XHT_TRADE_SESSION_H_

#include <deque>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "MyServer.h"
#include "semaphore.h"
#include "json.h"
#include "XtpTradeReq.h"
#include "XtpTradeRes.h"


#define HEADER_LENGTH 10
#define MAX_BODY_LENGTH 512
#define BUF_SIZE (HEADER_LENGTH + MAX_BODY_LENGTH)


class CTradeSession : public boost::enable_shared_from_this<CTradeSession>
{
public:
	CTradeSession(boost::asio::io_service& io_service, CMyServer &server, bool bTradeSession = true);
	void handle_read_header(const boost::system::error_code& error);
	void handle_read_body(const boost::system::error_code& error);
	void handle_write(const boost::system::error_code& error);

	// 请求和应答处理线程
	static void thread_handler_req(CTradeSession *pThis);
	static void thread_handler_res(CTradeSession *pThis);


	// 处理请求
	void handle_req(std::string strPacket);
	void handle_req_insert_order(Json::Value &v);
	void handle_req_cancel_order(Json::Value &v);
	void handle_req_entrust_query_for_entrustID(Json::Value &v);
	void handle_req_entrust_query(Json::Value &v);
	void handle_req_trade_query_for_tradeID(Json::Value &v);
	void handle_req_trade_query(Json::Value &v);
	void handle_req_hold_query(Json::Value &v);
	void handle_req_asset_detail(Json::Value &v);

public:
	void combo_head(Json::Value &vHead, std::string strPacketID, std::string strTradeCode,
		std::string strResCode, std::string strMsg);// 组合应答报文的头部

	bool pushReqPacket(std::string &strPacket);// 添加请求包
	bool popReqPacket(std::string &strPacket);	// 弹出并删除请求包

	bool pushResPacket(std::string &strPacket);// 添加应答包
	bool popResPacket(std::string &strPacket);	// 弹出并删除应答包

	void triggerReqSignal();// 触发请求信号
	void triggerResSignal();// 触发应答信号


	void send_error_res_packet(std::string strPacketID, std::string strTradeCode, std::string strResCode, std::string strMsg);// 发送请求失败的应答包

	uint64_t getSessionID();

public:
	void start();
	void send(const std::string& msg);
	boost::asio::ip::tcp::socket& get_socket();

private:
	boost::asio::ip::tcp::socket m_socket;		// 当前用户的通信套接字
	char m_szBuf[BUF_SIZE];						// 接收缓冲区
	uint64_t m_nSessionID;						// 交易前置与内部系统的会话ID
	CMyServer &m_server;						// 当前实例所属的服务器
	Json::Reader m_reader;

	boost::mutex m_mutexReq;					// 请求队列互斥量
	boost::mutex m_mutexRes;					// 响应队列互斥量
	semaphore m_reqSema;						// 请求包到达的信号（当有请求包到达时，该信号被触发）
	semaphore m_resSema;						// 应答包到达的信号（当有应答包到达时，该信号被触发）

	CXtpTradeReq   m_tradeReq;					// xtp交易接口
	CXtpTraderRes* m_pTradeSpi;                 // xtp应答接口

	std::deque<std::string> m_trade_req_queue;	// 请求队列
	std::deque<std::string> m_trade_res_queue;	// 响应队列
};

#endif