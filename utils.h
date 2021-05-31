#ifndef _UTILS_H
#define _UTILS_H

#include "notifier/notifier.h"
#include "market/market.h"
#include "comm/config.h"
#include <boost/asio.hpp>
#include "strategy/strategy.h"

class Utils {
public:
  Utils(const std::string &filename);
  int run();
  static int init(const std::string &filename);
  void trading_handler();
  void add_timer();
private:
  std::shared_ptr<boost::asio::io_context> context_;
  std::shared_ptr<market::Market> market_;
  std::shared_ptr<notifier::Notifier> notifier_;
  std::shared_ptr<grid_trading::trading> grid_trading_;
  std::shared_ptr<CommConfig> config_;
  std::shared_ptr<boost::asio::steady_timer> timer_;
};


#endif