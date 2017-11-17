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

	// �����Ӧ�����߳�
	static void thread_handler_req(CTradeSession *pThis);
	static void thread_handler_res(CTradeSession *pThis);


	// ��������
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
		std::string strResCode, std::string strMsg);// ���Ӧ���ĵ�ͷ��

	bool pushReqPacket(std::string &strPacket);// ��������
	bool popReqPacket(std::string &strPacket);	// ������ɾ�������

	bool pushResPacket(std::string &strPacket);// ���Ӧ���
	bool popResPacket(std::string &strPacket);	// ������ɾ��Ӧ���

	void triggerReqSignal();// ���������ź�
	void triggerResSignal();// ����Ӧ���ź�


	void send_error_res_packet(std::string strPacketID, std::string strTradeCode, std::string strResCode, std::string strMsg);// ��������ʧ�ܵ�Ӧ���

	uint64_t getSessionID();

public:
	void start();
	void send(const std::string& msg);
	boost::asio::ip::tcp::socket& get_socket();

private:
	boost::asio::ip::tcp::socket m_socket;		// ��ǰ�û���ͨ���׽���
	char m_szBuf[BUF_SIZE];						// ���ջ�����
	uint64_t m_nSessionID;						// ����ǰ�����ڲ�ϵͳ�ĻỰID
	CMyServer &m_server;						// ��ǰʵ�������ķ�����
	Json::Reader m_reader;

	boost::mutex m_mutexReq;					// ������л�����
	boost::mutex m_mutexRes;					// ��Ӧ���л�����
	semaphore m_reqSema;						// �����������źţ��������������ʱ�����źű�������
	semaphore m_resSema;						// Ӧ���������źţ�����Ӧ�������ʱ�����źű�������

	CXtpTradeReq   m_tradeReq;					// xtp���׽ӿ�
	CXtpTraderRes* m_pTradeSpi;                 // xtpӦ��ӿ�

	std::deque<std::string> m_trade_req_queue;	// �������
	std::deque<std::string> m_trade_res_queue;	// ��Ӧ����
};

#endif