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
	 * @brief ��ʼ��xtp���׽ӿ�
	 *
	 * @param [in] szKey		  �������Key������̩����
	 * @param [in] szFilePath     ����������Ϣ�ļ���Ŀ¼��һ����ʵ���ڵ��п�дȨ�޵�·����
	 * @param [in] nClientID      �ͻ�id���û��Զ���ģ�
	 *
	 * @return true-��ʼ���ɹ�  false-��ʼ��ʧ�ܣ�����getLastErrorMsg��ȡ������Ϣ
	 */
	bool init(const char *szKey, const char *szFilePath, int nClientID, CXtpTraderRes *pTradeSpi);

	/*
	* @name login
	*
	* @brief ��¼xtp���׷�����
	*
	* @param [in] szUserName	��¼��
	* @param [in] szPwd			��¼����
	* @param [in] szIP			xtp���׷�������IP
	* @param [in] nPort			xtp���׷������Ķ˿�
	*
	* @return ��0-��¼�ɹ�(�÷���ֵ�ǻỰID)  0-��¼ʧ�ܣ�����getLastErrorMsg��ȡ������Ϣ
	*/
	bool doLogin(const char *szUserName, const char *szPwd, const char *szIP, int nPort);

	// �ǳ�
	int loginout();

	// ί���µ�
	bool doEntrustOrder(XTPOrderInsertInfo *order, std::string strPacketID);

	// ί�г���
	bool doCancelEntrust(const uint64_t entrustID, std::string strPacketID);

	// ί�в�ѯ(���ݶ�������ѯ)
	bool doQueryEntrust(const XTPQueryOrderReq *queryParam, std::string strPacketID);

	// ί�в�ѯ(����ί��ID��ѯ)
	bool doQueryEntrustForID(const uint64_t entrustID, std::string strPacketID);

	// �ɽ���ѯ�����ݶ�������ѯ)
	bool doQueryTrade(XTPQueryTraderReq *queryParam, std::string strPacketID);

	// �ɽ���ѯ(���ݳɽ�ID��ѯ)
	bool doQueryTradeForID(const uint64_t tradeID, std::string strPacketID);

	// �ֲֲ�ѯ
	bool doQueryHold(const char *ticker, std::string strPacketID);

	// �ʲ���ѯ
	bool doQueryAssert(std::string strPacketID);

	// ��ȡ������Ϣ
	char* getLastErrorMsg();

	void setConnectStatus(bool bConnected);
	bool isConnected();

	std::string getPacketID(uint64_t key);
	void addPacketID(uint64_t key, std::string strPacketID);
	void deletePacketID(uint64_t key);

private:
	bool                  m_bLogined;					// �Ƿ��ѵ�¼xtp���׷�����
	char                  m_szErrorMsg[MAX_ERROR_MSG];	// API������ʧ��ʱ�ĵĴ�������
	XTP::API::TraderApi*  m_pXtpTradeReq;				// xtp�������
	
	uint64_t              m_uSessionID;					// ��xtp���׷�����ͨ�ŵĻỰID
	

	// first: ����xtpͨ�ŵ�Ψһ��ʾ������µ��ͳ�������ί��ID�򳷵�ID�����������ѯ������requestID
	// seccond: �ǽ���ǰ�úͽ���ϵͳ֮��ı���Ψһ��ʾ
	std::map<uint64_t, std::string> m_mapPacketID;      // ���������ʱ�ı���Ψһ��ʶ

};


#endif
