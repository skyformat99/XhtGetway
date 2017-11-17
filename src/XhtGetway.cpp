#include <iostream>
#include <boost/asio.hpp>
#include "../include/XhtGetwayApp.h"



int main()
{
	char szIP[16];
	int nTradePort, nQuotePort;
	
	if (!theApp.config.readConfig())
	{
		std::cout << "配置读取失败！" << std::endl;
		std::cout << std::endl << "按任意键退出." << std::endl;
		std::cin.get();
		return -1;
	}

	theApp.config.getTradeGewayAddr(szIP, MAX_IP, nTradePort);
	theApp.config.getQuoteGewayAddr(szIP, MAX_IP, nQuotePort);

	try
	{
		boost::asio::io_service io_service;

		// 交易服务的创建
		{
			boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(szIP), nTradePort);
			server_ptr tradeServer(new CMyServer(io_service, endpoint, TRADE_SERVER));// 启动交易监听服务
			theApp.setTradeServer(tradeServer);
		}

		// 行情服务的创建
		{
			boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(szIP), nQuotePort);
			server_ptr quoteServer(new CMyServer(io_service, endpoint, QUOTE_SERVER));// 启动行情监听服务
			theApp.setQuoteServer(quoteServer);
		}

		std::cout << "交易前置启动成功！" << std::endl;

		io_service.run();

		//tradeServer->m_tradeRequest.m_xtpTradeReq.loginout(uSession);
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	std::cout << std::endl << "按任意键退出." << std::endl;
	std::cin.get();

    return 0;
}

