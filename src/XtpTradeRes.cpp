#include <iostream>
#include <cstring>
#include <iomanip>
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
#include <assert.h>
#include "tradeFuncID.h"
#include "../include/XtpTradeRes.h"
#include "../include/XtpTradeReq.h"
#include "../include/XhtGetwayApp.h"
#include "../include/TradeSession.h"


#ifndef _WINDOWS
#include <sys/time.h>
#include <unistd.h>
#else
#include <windows.h>
#endif


CXtpTraderRes::CXtpTraderRes(CXtpTradeReq &xtpTradeReq, CTradeSession &owner)
	:m_xtpTradeReq(xtpTradeReq),
	m_owner(owner)
{

}

CXtpTraderRes::~CXtpTraderRes()
{
}

bool CXtpTraderRes::comboHead(Json::Value &vHead, uint64_t key, std::string strTradeCode, std::string strResCode/* = ""*/, std::string strMsg/* = ""*/)
{
	std::string strPacketID = m_xtpTradeReq.getPacketID(key);
	if (!strPacketID.empty())
	{
		time_t tt = time(NULL);
		tm* t = localtime(&tt);
		char szTime[32];
		sprintf(szTime, "%d%02d%02d%02d%02d%02d",
			t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
			t->tm_hour, t->tm_min, t->tm_sec);

		vHead["s_trd"] = strPacketID;
		vHead["s_trdcode"] = strTradeCode;
		vHead["s_trddatetime"] = szTime;
		vHead["s_respcode"] = strResCode;
		vHead["s_respdesc"] = strMsg;
	}
	else
	{
		return false;
	}

	return true;
}

void CXtpTraderRes::OnDisconnected(uint64_t session_id, int reason)
{
}

void CXtpTraderRes::OnError(XTPRI *error_info)
{
	//cout << "--->>> " << "OnRspError" << endl;
	IsErrorRspInfo(error_info);
}


void CXtpTraderRes::OnOrderEvent(XTPOrderInfo *order_info, XTPRI *error_info)
{
	//switch (order_info->order_status)
	//{
	//case XTP_ORDER_STATUS_NOTRADEQUEUEING:
	//{
	//	//pUserApi->CancelOrder(order_info->order_xtp_id, session_arrary[order_info->order_client_id]);
	//	break;
	//}
	//case XTP_ORDER_STATUS_ALLTRADED:
	//case XTP_ORDER_STATUS_PARTTRADEDNOTQUEUEING:
	//case XTP_ORDER_STATUS_CANCELED:
	//case XTP_ORDER_STATUS_REJECTED:
	//{
	//	//int i = order_info->order_client_id;
	//	//pUserApi->InsertOrder(&(orderList[i]), session_arrary[i]);
	//	break;
	//}
	//default:
	//	break;
	//}

	if (0 != error_info->error_id)
	{
		return;
	}

	char szBuf[32];

	Json::Value vHead ,vBody;

	sprintf(szBuf, "%ld", order_info->order_xtp_id);
	vBody["order_xtp_id"] = szBuf;

	sprintf(szBuf, "%d", order_info->order_client_id);
	vBody["order_client_id"] = szBuf;

	sprintf(szBuf, "%d", order_info->order_cancel_client_id);
	vBody["order_cancel_client_id"] = szBuf;

	sprintf(szBuf, "%ld", order_info->order_cancel_xtp_id);
	vBody["order_cancel_xtp_id"] = szBuf;

	vBody["ticker"] = order_info->ticker;

	sprintf(szBuf, "%d", order_info->market);
	vBody["market"] = szBuf;

	sprintf(szBuf, "%f", order_info->price);
	vBody["price"] = szBuf;

	sprintf(szBuf, "%ld", order_info->quantity);
	vBody["quantity"] = szBuf;

	sprintf(szBuf, "%d", order_info->price_type);
	vBody["price_type"] = szBuf;

	sprintf(szBuf, "%d", order_info->side);
	vBody["side"] = szBuf;

	sprintf(szBuf, "%d", order_info->business_type);
	vBody["business_type"] = szBuf;

	sprintf(szBuf, "%ld", order_info->qty_traded);
	vBody["qty_traded"] = szBuf;

	sprintf(szBuf, "%ld", order_info->qty_left);
	vBody["qty_left"] = szBuf;

	sprintf(szBuf, "%ld", order_info->insert_time);
	vBody["insert_time"] = szBuf;

	sprintf(szBuf, "%ld", order_info->update_time);
	vBody["update_time"] = szBuf;

	sprintf(szBuf, "%ld", order_info->cancel_time);
	vBody["cancel_time"] = szBuf;

	sprintf(szBuf, "%f", order_info->trade_amount);
	vBody["trade_amount"] = szBuf;

	vBody["order_local_id"] = order_info->order_local_id;

	sprintf(szBuf, "%d", order_info->order_status);
	vBody["order_status"] = szBuf;

	sprintf(szBuf, "%d", order_info->order_submit_status);
	vBody["order_submit_status"] = szBuf;

	sprintf(szBuf, "%d", order_info->order_type);
	vBody["order_type"] = szBuf;

	comboHead(vHead, order_info->order_xtp_id, TS_FUNC_INSERT_ORDER, "", "");

	Json::Value vPacket;
	vPacket["head"] = vHead;
	vPacket["body"] = vBody;

	std::string strOrderData = m_fastWriter.write(vPacket);

	if (m_owner.pushResPacket(strOrderData))
	{
		m_owner.triggerResSignal();
	}
}

void CXtpTraderRes::OnTradeEvent(XTPTradeReport *trade_info)
{
	char szBuf[32];

	Json::Value vHead, vBody;

	sprintf(szBuf, "%ld", trade_info->order_xtp_id);
	vBody["order_xtp_id"] = szBuf;

	sprintf(szBuf, "%d", trade_info->order_client_id);
	vBody["order_client_id"] = szBuf;

	vBody["ticker"] = trade_info->ticker;

	sprintf(szBuf, "%d", trade_info->market);
	vBody["market"] = szBuf;

	sprintf(szBuf, "%ld", trade_info->local_order_id);
	vBody["local_order_id"] = szBuf;

	vBody["exec_id"] = trade_info->exec_id;

	sprintf(szBuf, "%f", trade_info->price);
	vBody["price"] = szBuf;

	sprintf(szBuf, "%ld", trade_info->quantity);
	vBody["quantity"] = szBuf;

	sprintf(szBuf, "%ld", trade_info->trade_time);
	vBody["trade_time"] = szBuf;

	sprintf(szBuf, "%f", trade_info->trade_amount);
	vBody["trade_amount"] = szBuf;

	sprintf(szBuf, "%ld", trade_info->report_index);
	vBody["report_index"] = szBuf;

	vBody["order_exch_id"] = trade_info->order_exch_id;

	sprintf(szBuf, "%d", trade_info->trade_type);
	vBody["trade_type"] = szBuf;

	sprintf(szBuf, "%d", trade_info->side);
	vBody["side"] = szBuf;

	sprintf(szBuf, "%d", trade_info->business_type);
	vBody["business_type"] = szBuf;

	vBody["branch_pbu"] = trade_info->branch_pbu;

	comboHead(vHead, trade_info->order_xtp_id, TS_FUNC_INSERT_ORDER, "", "");

	Json::Value vPacket;
	vPacket["head"] = vHead;
	vPacket["body"] = vBody;

	std::string strTradeData = m_fastWriter.write(vPacket);

	
	if (m_owner.pushResPacket(strTradeData))
	{
		m_owner.triggerResSignal();
	}
}

void CXtpTraderRes::OnCancelOrderError(XTPOrderCancelInfo * cancel_info, XTPRI * error_info)
{
	Json::Value vBody;
	vBody["order_cancel_xtp_id"] = cancel_info->order_cancel_xtp_id;
	vBody["order_xtp_id"] = cancel_info->order_xtp_id;
}

void CXtpTraderRes::OnQueryOrder(XTPQueryOrderRsp * order_info, XTPRI * error_info, int request_id, bool is_last)
{
	char szBuf[32];

	Json::Value vHead, vBody;

	sprintf(szBuf, "%ld", order_info->order_xtp_id);
	vBody["order_xtp_id"] = szBuf;

	sprintf(szBuf, "%d", order_info->order_client_id);
	vBody["order_client_id"] = szBuf;

	sprintf(szBuf, "%d", order_info->order_cancel_client_id);
	vBody["order_cancel_client_id"] = szBuf;

	sprintf(szBuf, "%ld", order_info->order_cancel_xtp_id);
	vBody["order_cancel_xtp_id"] = szBuf;

	vBody["ticker"] = order_info->ticker;

	sprintf(szBuf, "%d", order_info->market);
	vBody["market"] = szBuf;

	sprintf(szBuf, "%f", order_info->price);
	vBody["price"] = szBuf;

	sprintf(szBuf, "%ld", order_info->quantity);
	vBody["quantity"] = szBuf;

	sprintf(szBuf, "%d", order_info->price_type);
	vBody["price_type"] = szBuf;

	sprintf(szBuf, "%d", order_info->side);
	vBody["side"] = szBuf;

	sprintf(szBuf, "%d", order_info->business_type);
	vBody["business_type"] = szBuf;

	sprintf(szBuf, "%ld", order_info->qty_traded);
	vBody["qty_traded"] = szBuf;

	sprintf(szBuf, "%ld", order_info->qty_left);
	vBody["qty_left"] = szBuf;

	sprintf(szBuf, "%ld", order_info->insert_time);
	vBody["insert_time"] = szBuf;

	sprintf(szBuf, "%ld", order_info->update_time);
	vBody["update_time"] = szBuf;

	sprintf(szBuf, "%ld", order_info->cancel_time);
	vBody["cancel_time"] = szBuf;

	sprintf(szBuf, "%f", order_info->trade_amount);
	vBody["trade_amount"] = szBuf;

	vBody["order_local_id"] = order_info->order_local_id;

	sprintf(szBuf, "%d", order_info->order_status);
	vBody["order_status"] = szBuf;

	sprintf(szBuf, "%d", order_info->order_submit_status);
	vBody["order_submit_status"] = szBuf;

	sprintf(szBuf, "%d", order_info->order_type);
	vBody["order_type"] = szBuf;

	comboHead(vHead, request_id, TS_FUNC_ENTRUST_QUERY, "", "");

	Json::Value vPacket;
	vPacket["head"] = vHead;
	vPacket["body"] = vBody;

	std::string strEntrustData = m_fastWriter.write(vPacket);

	if (m_owner.pushResPacket(strEntrustData))
	{
		m_owner.triggerResSignal();
	}

}

void CXtpTraderRes::OnQueryTrade(XTPQueryTradeRsp * trade_info, XTPRI * error_info, int request_id, bool is_last)
{
	char szBuf[32];

	Json::Value vHead, vBody;

	sprintf(szBuf, "%ld", trade_info->order_xtp_id);
	vBody["order_xtp_id"] = szBuf;

	sprintf(szBuf, "%d", trade_info->order_client_id);
	vBody["order_client_id"] = szBuf;

	vBody["ticker"] = trade_info->ticker;

	sprintf(szBuf, "%d", trade_info->market);
	vBody["market"] = szBuf;

	sprintf(szBuf, "%ld", trade_info->local_order_id);
	vBody["local_order_id"] = szBuf;

	vBody["exec_id"] = trade_info->exec_id;

	sprintf(szBuf, "%f", trade_info->price);
	vBody["price"] = szBuf;

	sprintf(szBuf, "%ld", trade_info->quantity);
	vBody["quantity"] = szBuf;

	sprintf(szBuf, "%ld", trade_info->trade_time);
	vBody["trade_time"] = szBuf;

	sprintf(szBuf, "%f", trade_info->trade_amount);
	vBody["trade_amount"] = szBuf;

	sprintf(szBuf, "%ld", trade_info->report_index);
	vBody["report_index"] = szBuf;

	vBody["order_exch_id"] = trade_info->order_exch_id;

	sprintf(szBuf, "%d", trade_info->trade_type);
	vBody["trade_type"] = szBuf;

	sprintf(szBuf, "%d", trade_info->side);
	vBody["side"] = szBuf;

	sprintf(szBuf, "%d", trade_info->business_type);
	vBody["business_type"] = szBuf;

	vBody["branch_pbu"] = trade_info->branch_pbu;

	comboHead(vHead, request_id, TS_FUNC_TRADE_QUERY, "", "");

	Json::Value vPacket;
	vPacket["head"] = vHead;
	vPacket["body"] = vBody;

	std::string strTradeData = m_fastWriter.write(vPacket);
	if (m_owner.pushResPacket(strTradeData))
	{
		m_owner.triggerResSignal();
	}
}

void CXtpTraderRes::OnQueryPosition(XTPQueryStkPositionRsp * investor_position, XTPRI * error_info, int request_id, bool is_last)
{

	if (0 != error_info->error_id)
	{
		//m_pOwner->onError(error_info->error_id, error_info->error_msg);
	}
	else
	{
		char szBuf[32];

		Json::Value vHead, vBody;

		vBody["ticker"] = investor_position->ticker;					// 证券代码
		vBody["ticker_name"] = investor_position->ticker_name;			// 证券名称

		sprintf(szBuf, "%d", investor_position->market);
		vBody["market"] = szBuf;										// 交易市场

		sprintf(szBuf, "%ld", investor_position->total_qty);
		vBody["total_qty"] = szBuf;										// 当前持仓

		sprintf(szBuf, "%ld", investor_position->sellable_qty);
		vBody["sellable_qty"] = szBuf;									// 可用股份数

		sprintf(szBuf, "%f", investor_position->avg_price);
		vBody["avg_price"] = szBuf;										// 买入成本价

		sprintf(szBuf, "%f", investor_position->unrealized_pnl);
		vBody["unrealized_pnl"] = szBuf;								// 浮动盈亏（保留字段）

		sprintf(szBuf, "%f", request_id);
		vBody["request_id"] = szBuf;

		comboHead(vHead, request_id, TS_FUNC_HOLD_QUERY, "", "");

		Json::Value vPacket;
		vPacket["head"] = vHead;
		vPacket["body"] = vBody;

		std::string strHoldData = m_fastWriter.write(vPacket);
		if (m_owner.pushResPacket(strHoldData))
		{
			m_owner.triggerResSignal();
		}
	}
}

void CXtpTraderRes::OnQueryAsset(XTPQueryAssetRsp * trading_account, XTPRI * error_info, int request_id, bool is_last)
{
	char szBuf[32];

	Json::Value vHead, vBody;

	sprintf(szBuf, "%f", trading_account->total_asset);
	vBody["total_asset"] = szBuf;

	sprintf(szBuf, "%f", trading_account->buying_power);
	vBody["buying_power"] = szBuf;

	sprintf(szBuf, "%f", trading_account->security_asset);
	vBody["security_asset"] = szBuf;

	sprintf(szBuf, "%f", trading_account->fund_buy_amount);
	vBody["fund_buy_amount"] = szBuf;

	sprintf(szBuf, "%f", trading_account->fund_buy_fee);
	vBody["fund_buy_fee"] = szBuf;

	sprintf(szBuf, "%f", trading_account->fund_sell_amount);
	vBody["fund_sell_amount"] = szBuf;

	sprintf(szBuf, "%f", trading_account->fund_sell_fee);
	vBody["fund_sell_fee"] = szBuf;

	sprintf(szBuf, "%f", trading_account->withholding_amount);
	vBody["withholding_amount"] = szBuf;

	comboHead(vHead, request_id, TS_FUNC_ASSET_DETAIL, "", "");

	Json::Value vPacket;
	vPacket["head"] = vHead;
	vPacket["body"] = vBody;

	std::string strAssetData = m_fastWriter.write(vPacket);
	if (m_owner.pushResPacket(strAssetData))
	{
		m_owner.triggerResSignal();
	}

}

bool CXtpTraderRes::IsErrorRspInfo(XTPRI *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->error_id != 0));
	if (bResult)
	{
		//cout << "--->>> ErrorID=" << pRspInfo->error_id << ", ErrorMsg=" << pRspInfo->error_msg << endl;
	}
		
	return bResult;
}