#include "okex_api.h"

#include "okex/okex.h"
#include <memory>

market::OkexApi::OkexApi(boost::asio::io_context &context, boost::json::value config) {
  // okex_ = new Okex::Okex(context, api_key, secret_key, passphrase);
  auto okex_config_obj = config.as_object();
  std::string okex_api_key = okex_config_obj["api_key"].as_string().c_str();
  std::string okex_secret_key = okex_config_obj["secret_key"].as_string().c_str();
  std::string okex_passphrase = okex_config_obj["passphrase"].as_string().c_str();
  bool is_simulated = okex_config_obj["simulate"].as_bool();
  auto market_okex = std::make_unique<Okex::Okex>(context, okex_api_key, okex_secret_key, okex_passphrase);
  if (is_simulated) market_okex->set_simulated_trading();
  
  okex_ = std::move(market_okex);
}

market::OkexApi::~OkexApi() {}

std::string market::OkexApi::get_ticker_price(const std::string &instId) {
  auto rdata = okex_->market_ticket(instId);
  return (*rdata)["last"];
}

int market::OkexApi::buy_market(const std::string &instId, const std::string &quantity, const std::string &price) {
  auto rdata = okex_->trade_order(instId, Okex::trade_order::tdMode::cash, Okex::trade_order::posSide::buy, Okex::trade_order::ordType::limit, quantity, price);
  return std::atoi((*rdata)["data"].c_str());
}

int market::OkexApi::sell_market(const std::string &instId, const std::string &quantity, const std::string &price) {
  auto rdata = okex_->trade_order(instId, Okex::trade_order::tdMode::cash, Okex::trade_order::posSide::sell, Okex::trade_order::ordType::limit, quantity, price);
  return std::atoi((*rdata)["data"].c_str());
}

boost::json::value market::OkexApi::init() {
  boost::json::value config_obj = {
    {"api_key", ""},
    {"secret_key", ""},
    {"passphrase", ""},
    {"simulate", false}
  };
  return config_obj;
}