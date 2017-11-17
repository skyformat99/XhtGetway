#ifndef TRADE_GETWAY_TRADE_RES_H_
#define TRADE_GETWAY_TRADE_RES_H_
#include <xtp_trader_api.h>
#include <commonType.h>
#include "DataType.h"
#include "XtpTradeRes.h"

class CXtpTraderRes;

class CXtpTradeReq
{
public:
	CXtpTradeReq();
	~CXtpTradeReq();

private:
	bool setLastErrorMsgV1(int nPos);
	bool setLastErrorMsgV2(const char *szErrorMsg);

	bool createCatalog(const char *szCatalog);

	long getRequestID();

public:

	/*
	 * @name init
	 *
	 * @brief 初始化xtp交易接口
	 *
	 * @param [in] szKey		  软件开发Key（由中泰给）
	 * @param [in] szFilePath     存贮订阅信息文件的目录（一个真实存在的有可写权限的路径）
	 * @param [in] nClientID      客户id（用户自定义的）
	 *
	 * @return true-初始化成功  false-初始化失败，调用getLastErrorMsg获取错误信息
	 */
	bool init(const char *szKey, const char *szFilePath, int nClientID, CXtpTraderRes *pTradeSpi);

	/*
	* @name login
	*
	* @brief 登录xtp交易服务器
	*
	* @param [in] szUserName	登录名
	* @param [in] szPwd			登录密码
	* @param [in] szIP			xtp交易服务器的IP
	* @param [in] nPort			xtp交易服务器的端口
	*
	* @return 非0-登录成功(该返回值是会话ID)  0-登录失败，调用getLastErrorMsg获取错误信息
	*/
	bool doLogin(const char *szUserName, const char *szPwd, const char *szIP, int nPort);

	// 登出
	int loginout();

	// 委托下单
	bool doEntrustOrder(XTPOrderInsertInfo *order, std::string strPacketID);

	// 委托撤单
	bool doCancelEntrust(const uint64_t entrustID, std::string strPacketID);

	// 委托查询(根据多条件查询)
	bool doQueryEntrust(const XTPQueryOrderReq *queryParam, std::string strPacketID);

	// 委托查询(根据委托ID查询)
	bool doQueryEntrustForID(const uint64_t entrustID, std::string strPacketID);

	// 成交查询（根据多条件查询)
	bool doQueryTrade(XTPQueryTraderReq *queryParam, std::string strPacketID);

	// 成交查询(根据成交ID查询)
	bool doQueryTradeForID(const uint64_t tradeID, std::string strPacketID);

	// 持仓查询
	bool doQueryHold(const char *ticker, std::string strPacketID);

	// 资产查询
	bool doQueryAssert(std::string strPacketID);

	// 获取错误信息
	char* getLastErrorMsg();

	void setConnectStatus(bool bConnected);
	bool isConnected();

	std::string getPacketID(uint64_t key);
	void addPacketID(uint64_t key, std::string strPacketID);
	void deletePacketID(uint64_t key);

private:
	bool                  m_bLogined;					// 是否已登录xtp交易服务器
	char                  m_szErrorMsg[MAX_ERROR_MSG];	// API最后调用失败时的的错误描述
	XTP::API::TraderApi*  m_pXtpTradeReq;				// xtp请求对象
	
	uint64_t              m_uSessionID;					// 和xtp交易服务器通信的会话ID
	

	// first: 是与xtp通信的唯一标示。针对下单和撤单，是委托ID或撤单ID；针对其它查询请求，是requestID
	// seccond: 是交易前置和交易系统之间的报文唯一标示
	std::map<uint64_t, std::string> m_mapPacketID;      // 保存的请求时的报文唯一标识

};


#endif
