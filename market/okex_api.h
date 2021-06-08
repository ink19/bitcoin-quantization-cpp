#ifndef _MARKET_OKEX_API_H
#define _MARKET_OKEX_API_H

#include "market_impl/market_impl.h"
#include "okex/okex.h"
#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <memory>

namespace market {

class OkexApi : public market_impl {
public:
  OkexApi(boost::asio::io_context &context, boost::json::value config);
  ~OkexApi();
  std::string get_ticker_price(const std::string &instId);
  int buy_market(const std::string &instId, const std::string &quantity, const std::string &price);
  int sell_market(const std::string &instId, const std::string &quantity, const std::string &price);
  std::string get_trading_fee(const std::string &instId);
  static boost::json::value init();
private:
  std::unique_ptr<Okex::Okex> okex_;
};

}

#endif