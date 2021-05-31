#include "market.h"
#include <boost/asio.hpp>
#include <boost/json.hpp>
#include "okex_api.h"
#include <exception>

market::Market::Market(boost::asio::io_context &context, boost::json::value &market_config) {
  auto market_obj = market_config.as_object();
  if (market_obj["market"] == "okex") {
    auto okex_config_obj = market_obj["okex"].as_object();
    std::string okex_api_key = okex_config_obj["api_key"].as_string().c_str();
    std::string okex_secret_key = okex_config_obj["secret_key"].as_string().c_str();
    std::string okex_passphrase = okex_config_obj["passphrase"].as_string().c_str();
    bool is_simulated = okex_config_obj["simulate"].as_bool();
    auto market_okex = std::make_unique<OkexApi>(context, okex_api_key, okex_secret_key, okex_passphrase);
    if (is_simulated) market_okex->okex_->set_simulated_trading();
    
    market_ = std::move(market_okex);
  } else {
    std::cout << "Init Market Error." << std::endl;
    throw std::exception();
  }
}

market::Market::~Market() {}

std::string market::Market::get_ticker_price(const std::string &instId) {
  return market_->get_ticker_price(instId);
}

int market::Market::buy_market(const std::string &instId, const std::string &quantity, const std::string &price) {
  return market_->buy_market(instId, quantity, price);
}

int market::Market::sell_market(const std::string &instId, const std::string &quantity, const std::string &price) {
  return market_->sell_market(instId, quantity, price);
}

boost::json::value market::Market::init() {
  boost::json::value config_obj = {
    {"market", "okex"},
    {"okex", {
      {"api_key", ""},
      {"secret_key", ""},
      {"passphrase", ""},
      {"simulate", false}
    }}
  };
  return config_obj;
}