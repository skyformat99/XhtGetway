#ifndef TRADE_GETWAY_XTP_TRADE_RES_H_
#define TRADE_GETWAY_XTP_TRADE_RES_H_
#include <map>
#include "xtp_trader_api.h"
#include "DataType.h"
#include "json.h"

using namespace XTP::API;

class CXtpTradeReq;
class CTradeSession;


class CXtpTraderRes : public TraderSpi
{
public:
	Json::FastWriter m_fastWriter;
	CXtpTradeReq     &m_xtpTradeReq;
	CTradeSession    &m_owner;

public:
	CXtpTraderRes(CXtpTradeReq &xtpTradeReq, CTradeSession &owner);
	~CXtpTraderRes();

	bool comboHead(Json::Value &vHead, uint64_t key, std::string strTradeCode, std::string strResCode/* = ""*/, std::string strMsg/* = ""*/);

	virtual void OnDisconnected(uint64_t session_id, int reason);

	///错误应答
	virtual void OnError(XTPRI *error_info);

	///报单通知
	virtual void OnOrderEvent(XTPOrderInfo *order_info, XTPRI *error_info);

	///成交通知
	virtual void OnTradeEvent(XTPTradeReport *trade_info);

	virtual void OnCancelOrderError(XTPOrderCancelInfo *cancel_info, XTPRI *error_info);

	virtual void OnQueryOrder(XTPQueryOrderRsp *order_info, XTPRI *error_info, int request_id, bool is_last);

	virtual void OnQueryTrade(XTPQueryTradeRsp *trade_info, XTPRI *error_info, int request_id, bool is_last);

	virtual void OnQueryPosition(XTPQueryStkPositionRsp *investor_position, XTPRI *error_info, int request_id, bool is_last);

	virtual void OnQueryAsset(XTPQueryAssetRsp *trading_account, XTPRI *error_info, int request_id, bool is_last);
	

private:
	bool IsErrorRspInfo(XTPRI *pRspInfo);
};

#endif