#include "../include/XtpQuoteRes.h"
#include "../include/XhtGetwayApp.h"

#include "../include/QuoteSession.h"



void MyQuoteSpi::OnError(XTPRI *error_info, bool is_last)
{
	// ���ErrorID != 0, ˵���յ��˴������Ӧ
	bool bResult = ((error_info) && (error_info->error_id != 0));
	if (bResult)
	{
		// cout << "--->>> ErrorID=" << pRspInfo->error_id << ", ErrorMsg=" << pRspInfo->error_msg << endl;
	}
}

MyQuoteSpi::MyQuoteSpi(CQuoteSession &owner)
	:m_owner(owner)
{
}

MyQuoteSpi::~MyQuoteSpi()
{
}

void MyQuoteSpi::OnDisconnected(int reason)
{
}

void MyQuoteSpi::OnSubMarketData(XTPST *ticker, XTPRI *error_info, bool is_last)
{
	if (0 != error_info->error_id)
	{
		//printf("%s ����ʧ�ܣ�\n", ticker->ticker);
	}
}

void MyQuoteSpi::OnUnSubMarketData(XTPST *ticker, XTPRI *error_info, bool is_last)
{
}

void MyQuoteSpi::OnMarketData(XTPMD *market_data)
{
	Json::Value vHead, vBody;

	char szBuf[32] = {0};

	vBody["ticker"] = market_data->ticker;// ��Լ��

	sprintf(szBuf, "", market_data->last_price);
	vBody["last_price"] = szBuf;// ���¼�

	sprintf(szBuf, "", market_data->pre_close_price);
	vBody["pre_close_price"] = szBuf;// ������

	sprintf(szBuf, "", market_data->open_price);
	vBody["open_price"] = szBuf;// ����

	sprintf(szBuf, "", market_data->high_price);
	vBody["high_price"] = szBuf;// ��߼�

	sprintf(szBuf, "", market_data->low_price);
	vBody["low_price"] = szBuf;// ��ͼ�

	sprintf(szBuf, "", market_data->close_price);
	vBody["close_price"] = szBuf;// ������

	std::string strBid, strAsk, strBidQty, strAskQty;
	for (int i = 0; i < 10; ++i)
	{
		sprintf(szBuf, "%f,", market_data->bid[i]);//ʮ�������
		strBid += szBuf;

		sprintf(szBuf, "%f,", market_data->ask[i]);//ʮ��������
		strAsk += szBuf;

		sprintf(szBuf, "%lld,", market_data->bid_qty[i]);//ʮ��������
		strBidQty += szBuf;

		sprintf(szBuf, "%lld,", market_data->ask_qty[i]);//ʮ��������
		strAskQty += szBuf;
	}
	strBid = strBid.substr(0, strBid.length() - 1);
	strAsk = strAsk.substr(0, strAsk.length() - 1);
	strBidQty = strBidQty.substr(0, strBidQty.length() - 1);
	strAskQty = strAskQty.substr(0, strAskQty.length() - 1);

	vBody["bid"] = strBid;
	vBody["ask"] = strAsk;
	vBody["bid_qty"] = strBidQty;
	vBody["ask_qty"] = strAskQty;


	Json::Value vPacket;
	vPacket["head"] = vHead;
	vPacket["body"] = vBody;

	std::string strPacket = m_fWriter.write(vPacket);
	if (!strPacket.empty())
	{
		m_owner.pushResPacket(strPacket);
		m_owner.triggerResSignal();
	}
}

void MyQuoteSpi::OnQueryAllTickers(XTPQSI * ticker_info, XTPRI * error_info, bool is_last)
{
}

