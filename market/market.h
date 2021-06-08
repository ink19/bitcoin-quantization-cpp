#ifndef _MARKET_MARKET_H
#define _MARKET_MARKET_H

#include "market_impl/market_impl.h"
#include "okex_api.h"
#include <memory>
#include <boost/json.hpp>
#include <boost/asio.hpp>

namespace market {

class Market : public market_impl {
public:
  Market(boost::asio::io_context &context, boost::json::value &market_config, std::string market_type = "");
  // 对配置文件的初始化函数
  static boost::json::value init();

  ~Market();
  std::string get_ticker_price(const std::string &instId);
  int buy_market(const std::string &instId, const std::string &quantity, const std::string &price);
  int sell_market(const std::string &instId, const std::string &quantity, const std::string &price);
  std::string get_trading_fee(const std::string &instId);
private:
  std::unique_ptr<market_impl> market_;
};

}

#endif