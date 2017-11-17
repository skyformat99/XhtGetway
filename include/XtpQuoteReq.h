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
	uint64_t m_uQuoteSession;          // ��xtp���������ͨ�ŵĻỰID
	XTP::API::QuoteApi* m_pQuoteApi;   // xtp��������ӿڶ���
	//MyQuoteSpi *m_pQuoteSpi;           // xtp����Ӧ��ӿڶ���

	std::set<T_SubTimers> m_setSubTickersSZ;// ���涩�ĵ����й�Ʊ����
	std::set<T_SubTimers> m_setSubTickersSH;// ���涩�ĵĻ��й�Ʊ����


public:
	CXtpQuoteReq();
	~CXtpQuoteReq();

	void init(int nClientID, const char *szQuoteCatalog, MyQuoteSpi *pQuoteSpi);
	uint64_t doLogin(const char *szUserName, const char *szPwd, const char *szIP, int nPort);

	/*
	* @name loginout
	*
	* @brief �ǳ�
	*
	* @return 0-�ǳ��ɹ�  ��0-�ǳ�ʧ��
	*/
	int doLoginout();

	/*
	 * @name subQuote
	 * 
	 * @brief ��������
	 *
	 * @param szticker �Զ��ŷָ��Ĺ�Ʊ����
	 * @param ���������͡� 1-��֤������  2-��֤������
	 *
	 * @return true-���ĳɹ�  false-����ʧ��
	 */
	bool doSubQuote(const char *szTicker);

	/*
	* @name unSubQuote
	*
	* @brief ȡ������Ķ���
	*
	* @param szticker �Զ��ŷָ��Ĺ�Ʊ����
	* @param ���������͡� 1-��֤������  2-��֤������
	*
	* @return true-���ĳɹ�  false-����ʧ��
	*/
	int doUnSubQuote(const char *szTicker);
};


#endif

