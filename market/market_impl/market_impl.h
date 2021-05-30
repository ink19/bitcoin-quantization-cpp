#ifndef _MARKET_MARKET_IMPL_MARKET_IMPL_H
#define _MARKET_MARKET_IMPL_MARKET_IMPL_H

#include <string>
#include <iostream>

namespace market {

class market_impl {
public:
  virtual ~market_impl();
  virtual double get_ticker_price(const std::string &instId);
  virtual int buy_market(const std::string &instId, const std::string &quantity, const std::string &price);
  virtual int sell_market(const std::string &instId, const std::string &quantity, const std::string &price);
};

}

#endif