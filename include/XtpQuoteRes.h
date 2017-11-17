#pragma once
#include "xtp_quote_api.h"
#include <fstream>
#include <time.h>

#include <sys/timeb.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif // _WIN32

#include "json.h"

#define NUM_OF_ROUND 1

using namespace XTP::API;


class CQuoteSession;

class MyQuoteSpi : public QuoteSpi
{
public:
	MyQuoteSpi(CQuoteSession &owner);
	~MyQuoteSpi();

	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	///@param reason ����ԭ��
	///        0x1001 �����ʧ��
	///        0x1002 ����дʧ��
	///        0x2001 ����������ʱ
	///        0x2002 ��������ʧ��
	///        0x2003 �յ�������
	virtual void OnDisconnected(int reason);

///����Ӧ��
	virtual void OnError(XTPRI *error_info,bool is_last);

	///��������Ӧ��
	virtual void OnSubMarketData(XTPST *ticker, XTPRI *error_info, bool is_last);

	///ȡ����������Ӧ��
	virtual void OnUnSubMarketData(XTPST *ticker, XTPRI *error_info, bool is_last);

	///�������֪ͨ
	virtual void OnMarketData(XTPMD *market_data);

	///��ѯ�ɽ��׺�Լ��Ӧ��
	virtual void OnQueryAllTickers(XTPQSI* ticker_info, XTPRI *error_info, bool is_last);

public:
	CQuoteSession &m_owner;
	Json::FastWriter m_fWriter;
};