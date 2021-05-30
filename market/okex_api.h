#ifndef _MARKET_OKEX_API_H
#define _MARKET_OKEX_API_H

#include "market_impl/market_impl.h"
#include "okex/okex.h"
#include <boost/asio.hpp>

namespace market {

class OkexApi : public market_impl {
public:
  OkexApi(boost::asio::io_context &context, const std::string &api_key,
       const std::string &secret_key, const std::string &passphrase);
  ~OkexApi();
  double get_ticker_price(const std::string &instId);
  int buy_market(const std::string &instId, const std::string &quantity, const std::string &price);
  int sell_market(const std::string &instId, const std::string &quantity, const std::string &price);
private:
  Okex::Okex *okex_;
};

}

#endif