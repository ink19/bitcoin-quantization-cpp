#include "market/okex/okex.h"

#include <chrono>
#include <iomanip>
#include <ctime>
#include <iostream>
#include <boost/asio.hpp>

int main(int argc, char *args[]) {
  boost::asio::io_context ioc;

  Okex::Okex okex_api(ioc, "5679ee08-cd4d-4237-875d-c53928290e74", "05AF8EBBDD3817D607F973AEB4C6305E", "wugerong");

  auto data = okex_api.account_balance();

  for (auto idata : *data) {
    std::cout << idata.first << ", " << idata.second << std::endl;
  }

  return 0;
}