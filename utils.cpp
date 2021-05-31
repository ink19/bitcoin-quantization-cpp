#include "utils.h"

#include <boost/json.hpp>
#include <fstream>
#include <iostream>
#include "strategy/strategy.h"
#include <boost/bind/bind.hpp>


Utils::Utils(const std::string &filename) {
  boost::json::stream_parser json_parser;
  std::ifstream config_in;
  config_in.open(filename);
  std::string read_data;
  json_parser.reset();
  while (config_in >> read_data) {
    json_parser.write(read_data);
  }
  json_parser.finish();
  // json_parser.finish();
  config_in.close();

  boost::json::object all_config_json = json_parser.release().as_object();

  context_ = std::make_shared<boost::asio::io_context>();

  market_ = std::make_shared<market::Market>(*context_, all_config_json["market"]);
  notifier_ = std::make_shared<notifier::Notifier>(*context_, all_config_json["notifier"]);
  config_ = std::make_shared<CommConfig>(all_config_json["comm"]);
  // trading set
  grid_trading_ = strategy::Strategy<grid_trading::trading>::get_inst(all_config_json["strategy"]);
  grid_trading_->set_trading_fun([this](int side, const std::string &size, const std::string &price){
    if (side == grid_trading::trading_side::buy) {
      market_->buy_market(config_->trading_pair, size, price);
    } else if (side == grid_trading::trading_side::sell) {
      market_->sell_market(config_->trading_pair, size, price);
    }
    return 0;
  });
  grid_trading_->set_send_message_fun([this](const std::string &level, const std::string &message) {
    return notifier_->send_message(level, message);
  });

  // event loop
  timer_ = std::make_shared<boost::asio::steady_timer>(*context_);
  add_timer();
}

void Utils::add_timer() {
  timer_->expires_after(boost::asio::chrono::milliseconds(static_cast<int>(1000 * config_->refresh_interval)));
  timer_->async_wait(boost::bind(&Utils::trading_handler, this));
}

void Utils::trading_handler() {
  grid_trading_->commit_price(market_->get_ticker_price(config_->trading_pair));
  add_timer();
}


int Utils::run() {
  return context_->run();
}

int Utils::init(const std::string &filename) {
  boost::json::value json_data = {
    {"market", market::Market::init()},
    {"notifier", notifier::Notifier::init()},
    {"strategy", strategy::Strategy<grid_trading::trading>::init()},
    {"comm", CommConfig::init()}
  };

  std::string config_str = boost::json::serialize(json_data);
  
  std::ofstream config_out;
  config_out.open(filename);
  
  config_out << config_str;
  return 0;
}