#include "config.h"

#include <boost/json.hpp>
#include <fstream>
#include <iostream>

CommConfig::CommConfig(boost::json::value config) {
  auto config_obj = config.as_object();
  trading_pair = config_obj["goods"].as_string().c_str();
  refresh_interval = config_obj["refresh_interval"].as_double();
}

CommConfig::~CommConfig(){}

boost::json::value CommConfig::init() {
  boost::json::value json_data = {
    {"refresh_interval", 1.0},
    {"goods", "ETH-USDT"}
  };
  return json_data;
}