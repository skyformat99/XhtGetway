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

	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	///@param reason 错误原因
	///        0x1001 网络读失败
	///        0x1002 网络写失败
	///        0x2001 接收心跳超时
	///        0x2002 发送心跳失败
	///        0x2003 收到错误报文
	virtual void OnDisconnected(int reason);

///错误应答
	virtual void OnError(XTPRI *error_info,bool is_last);

	///订阅行情应答
	virtual void OnSubMarketData(XTPST *ticker, XTPRI *error_info, bool is_last);

	///取消订阅行情应答
	virtual void OnUnSubMarketData(XTPST *ticker, XTPRI *error_info, bool is_last);

	///深度行情通知
	virtual void OnMarketData(XTPMD *market_data);

	///查询可交易合约的应答
	virtual void OnQueryAllTickers(XTPQSI* ticker_info, XTPRI *error_info, bool is_last);

public:
	CQuoteSession &m_owner;
	Json::FastWriter m_fWriter;
};