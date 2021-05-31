#include "okex_api.h"

#include "okex/okex.h"

market::OkexApi::OkexApi(boost::asio::io_context &context, const std::string &api_key,
       const std::string &secret_key, const std::string &passphrase) {
  okex_ = new Okex::Okex(context, api_key, secret_key, passphrase);
}

market::OkexApi::~OkexApi() {
  delete okex_;
}

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