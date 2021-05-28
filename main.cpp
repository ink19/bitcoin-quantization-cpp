#include "market/okex/okex.h"

#include <chrono>
#include <iomanip>
#include <ctime>
#include <iostream>
#include <boost/asio.hpp>


int main(int argc, char *args[]) {
  boost::asio::io_context ioc;

  Okex::Okex okex_api(ioc, "93ecaf98-4801-4a28-b064-3f08debff58c", "2129CD3B33B6C3C6179D455D4976FABA", "wugerong");
  // Okex::Okex okex_api(ioc, "5679ee08-cd4d-4237-875d-c53928290e74", "05AF8EBBDD3817D607F973AEB4C6305E", "wugerong");
  okex_api.set_simulated_trading();
  auto data = okex_api.trade_order("ETH-USDT", "cash", "buy", "limit", "0.01", "2000");
  // auto data = okex_api.account_balance();
  for (auto idata : *data) {
    std::cout << idata.first << ", " << idata.second << std::endl;
  }

  return 0;
}