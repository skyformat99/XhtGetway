#ifndef XHT_GETWAY_QUOTE_REQ_H_
#define XHT_GETWAY_QUOTE_REQ_H_
#include "xtp_quote_api.h"
#include "XtpQuoteRes.h"
#include "DataType.h"
#include <set>

typedef struct _SubTimers
{
	std::string strTicker;
	int nTimers;
	_SubTimers()
	{
		nTimers = 0;
	}

	bool operator < (const _SubTimers &subTimers) const 
	{
		return subTimers.strTicker > strTicker;
	}
}T_SubTimers;


class CXtpQuoteReq
{
public:
	uint64_t m_uQuoteSession;          // 和xtp行情服务器通信的会话ID
	XTP::API::QuoteApi* m_pQuoteApi;   // xtp行情请求接口对象
	//MyQuoteSpi *m_pQuoteSpi;           // xtp行情应答接口对象

	std::set<T_SubTimers> m_setSubTickersSZ;// 保存订阅的深市股票代码
	std::set<T_SubTimers> m_setSubTickersSH;// 保存订阅的沪市股票代码


public:
	CXtpQuoteReq();
	~CXtpQuoteReq();

	void init(int nClientID, const char *szQuoteCatalog, MyQuoteSpi *pQuoteSpi);
	uint64_t doLogin(const char *szUserName, const char *szPwd, const char *szIP, int nPort);

	/*
	* @name loginout
	*
	* @brief 登出
	*
	* @return 0-登出成功  非0-登出失败
	*/
	int doLoginout();

	/*
	 * @name subQuote
	 * 
	 * @brief 订阅行情
	 *
	 * @param szticker 以逗号分隔的股票代码
	 * @param 交易所类型。 1-上证交易所  2-深证交易所
	 *
	 * @return true-订阅成功  false-订阅失败
	 */
	bool doSubQuote(const char *szTicker);

	/*
	* @name unSubQuote
	*
	* @brief 取消行情的订阅
	*
	* @param szticker 以逗号分隔的股票代码
	* @param 交易所类型。 1-上证交易所  2-深证交易所
	*
	* @return true-订阅成功  false-订阅失败
	*/
	int doUnSubQuote(const char *szTicker);
};


#endif

