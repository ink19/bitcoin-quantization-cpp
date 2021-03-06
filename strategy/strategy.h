#ifndef _STRATEGY_STRATEGY_H
#define _STRATEGY_STRATEGY_H

#include "grid_trading/trading.h"
#include <boost/json.hpp>
#include <memory>

namespace strategy {

template <typename T>
class Strategy {
public:
  static boost::json::value init();
  std::shared_ptr<T> get_inst(const boost::json::value &config);
};

template <>
class Strategy<grid_trading::trading> {
public:
  static boost::json::value init() {
    boost::json::value config_obj = {
      {
        "grid_trading", {
          {"quantity", {"10", "15", "20"}},
          {"grid_size", "0.02"}
        }
      }
    };
    grid_trading::trading::init();

    return config_obj;
  }

  static std::shared_ptr<grid_trading::trading> get_inst(const boost::json::value &config) {
    auto config_obj = config.as_object();
    config_obj = config_obj["grid_trading"].as_object();
    auto res = std::make_shared<grid_trading::trading>();
    // std::string quantity(config_obj["quantity"].as_string().c_str());
    std::vector<std::string> quantity;

    for (auto quantity_item : config_obj["quantity"].as_array()) {
      quantity.push_back({quantity_item.as_string().c_str()});
    }

    std::string grid_size(config_obj["grid_size"].as_string().c_str());
    res->quantity(quantity);
    res->grid_size(grid_size);
    
    return res;
  }
};

}

#endif