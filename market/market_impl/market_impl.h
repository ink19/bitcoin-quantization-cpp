#ifndef _MARKET_MARKET_IMPL_MARKET_IMPL_H
#define _MARKET_MARKET_IMPL_MARKET_IMPL_H

#include <iostream>
#include <string>

namespace market {

class market_impl {
public:
  virtual ~market_impl(){};
  virtual std::string get_ticker_price(const std::string &instId) {
    return "";
  };
  virtual int buy_market(const std::string &instId, const std::string &quantity,
                         const std::string &price) {
    return 0;
  };
  virtual int sell_market(const std::string &instId,
                          const std::string &quantity,
                          const std::string &price) {
    return 0;
  };
};

} // namespace market

#endif