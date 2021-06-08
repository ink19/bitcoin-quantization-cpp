#include "market.h"
#include <boost/asio.hpp>
#include <boost/json.hpp>
#include "okex_api.h"
#include <exception>
#include "fake.h"

market::Market::Market(boost::asio::io_context &context, boost::json::value &market_config, std::string market_type) {
  auto market_obj = market_config.as_object();

  if (market_type == "") {
    market_type = market_obj["market"].as_string().c_str();
  }

  if (market_type == "okex") {
    auto okex_config_obj = market_obj["okex"];
    market_ = std::make_unique<OkexApi>(context, okex_config_obj);
  } else if (market_type == "fake") {
    auto fake_config_obj = market_obj["fake"];
    auto fake_market_ = std::make_unique<FakeMarket>(fake_config_obj);
    fake_market_->set_market(std::make_unique<Market>(context, market_config, fake_config_obj.as_object()["market"].as_string().c_str()));
    market_ = std::move(fake_market_);
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

std::string market::Market::get_trading_fee(const std::string &instId) {
  return market_->get_trading_fee(instId);
}

boost::json::value market::Market::init() {
  boost::json::value config_obj = {
    {"market", "okex"},
    {"okex", OkexApi::init()},
    {"fake", FakeMarket::init()}
  };
  return config_obj;
}