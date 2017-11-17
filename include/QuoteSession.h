#ifndef XHT_QUOTE_SESSION_H_
#define XHT_QUOTE_SESSION_H_

#include <deque>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "MyServer.h"
#include "semaphore.h"
#include "json.h"
#include "xtp_api_struct.h"
#include "XtpQuoteReq.h"


#define HEADER_LENGTH 10
#define MAX_BODY_LENGTH 512
#define BUF_SIZE (HEADER_LENGTH + MAX_BODY_LENGTH)


class CQuoteSession : public boost::enable_shared_from_this<CQuoteSession>
{
public:
	CQuoteSession(boost::asio::io_service& io_service, CMyServer &server, bool bTradeSession = true);
	void handle_read_header(const boost::system::error_code& error);
	void handle_read_body(const boost::system::error_code& error);
	void handle_write(const boost::system::error_code& error);

	// �����Ӧ�����߳�
	static void thread_handler_req(CQuoteSession *pThis);
	static void thread_handler_res(CQuoteSession *pThis);

	void handle_req(std::string strPacket);
	void handle_req_quote_sub(Json::Value &v);
	void handle_req_cancel_quote_sub(Json::Value &v);

public:
	void combo_head(Json::Value &vHead, std::string strPacketID, std::string strTradeCode,
		std::string strResCode, std::string strMsg);// ���Ӧ���ĵ�ͷ��

	bool pushReqPacket(std::string &strPacket);// ��������
	bool popReqPacket(std::string &strPacket);	// ������ɾ�������

	void pushResPacket(std::string &strPacket);// ���Ӧ���
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

	std::deque<std::string> m_quote_req_queue;	// �������
	std::deque<std::string> m_quote_res_queue;	// ��Ӧ����

	CXtpQuoteReq   m_quoteReq;
	MyQuoteSpi     *m_pQuoteSpi;

};

#endif