#ifndef _MARKET_FAKE_FAKE_H
#define _MARKET_FAKE_FAKE_H

#include "market_impl/market_impl.h"
#include <memory>
#include <boost/json.hpp>
#include <iostream>
#include <fstream>
#include <map>
#include <boost/multiprecision/cpp_dec_float.hpp>

namespace market {

// 替换掉交易部分，只保留信息获取部分
class FakeMarket : public market_impl {
public:
  FakeMarket(boost::json::value config);
  static boost::json::value init();

  std::string get_ticker_price(const std::string &instId) {
    return market_->get_ticker_price(instId);
  };
  int buy_market(const std::string &instId, const std::string &quantity,
                         const std::string &price);
  int sell_market(const std::string &instId,
                          const std::string &quantity,
                          const std::string &price);
  int set_market(std::unique_ptr<market_impl>&& market) {
    market_ = std::move(market);
    return 0;
  }
  std::string get_trading_fee(const std::string &instId) {
    return market_->get_trading_fee(instId);
  }
private:
  std::shared_ptr<market_impl> market_;
  std::ofstream out_stream_;
  std::map<std::string, boost::multiprecision::cpp_dec_float_100> account_;
  boost::multiprecision::cpp_dec_float_100 balance_;
};

};

#endif