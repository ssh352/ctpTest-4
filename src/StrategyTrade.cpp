#include <vector>
#include <string>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <iostream>
#include "StrategyTrade.h"
#include "CustomTradeSpi.h"

extern std::unordered_map<std::string, TickToKlineHelper> g_KlineHash;

// 线程互斥量
std::mutex marketDataMutex;

void StrategyCheckAndTrade(TThostFtdcInstrumentIDType instrumentID, CustomTradeSpi *customTradeSpi)
{
	// 加锁
	std::lock_guard<std::mutex> lk(marketDataMutex);
	TickToKlineHelper tickToKlineObject = g_KlineHash.at(std::string(instrumentID));
	// 策略
	std::vector<double> priceVec = tickToKlineObject.m_priceVec;
	if (priceVec.size() >= 3)
	{
		std::cout << "开仓----------------------" << std::endl;
		int len = priceVec.size();
		// 最后连续三个上涨就买开仓,反之就卖开仓,这里暂时用最后一个价格下单
		if (priceVec[len - 1] > priceVec[len - 2] && priceVec[len - 2] > priceVec[len - 3])
			customTradeSpi->reqOrderInsert(instrumentID, priceVec[len - 1], 1, THOST_FTDC_D_Buy);
		else if (priceVec[len - 1] < priceVec[len - 2] && priceVec[len - 2] < priceVec[len - 3])
			customTradeSpi->reqOrderInsert(instrumentID, priceVec[len - 1], 1, THOST_FTDC_D_Buy);
	}
}
