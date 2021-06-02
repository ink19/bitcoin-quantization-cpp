#include "fake.h"
#include <boost/format.hpp>
#include <iostream>

market::FakeMarket::FakeMarket(boost::json::value config) {
  auto config_obj = config.as_object();
  std::string out_file_name = config_obj["out_file"].as_string().c_str();
  std::string balance_s = config_obj["balance"].as_string().c_str();
  out_stream_.open(out_file_name);
  balance_ = boost::multiprecision::cpp_dec_float_100(balance_s);
}

int market::FakeMarket::buy_market(const std::string &instId, const std::string &quantity,
               const std::string &price) {
  boost::multiprecision::cpp_dec_float_100 quantity_f(quantity);
  boost::multiprecision::cpp_dec_float_100 price_f(price);
  boost::multiprecision::cpp_dec_float_100 require_m = quantity_f * price_f;

  if (require_m > balance_) {
    out_stream_ << boost::format("以%1%的价格购买%2%的%3%失败，余额不足.")%price%quantity%instId << std::endl;
    return -1;
  }
  
  balance_ -= require_m;

  if (account_.count(instId) == 0) {
    account_[instId] = quantity_f;
  } else {
    account_[instId] += quantity_f;
  }

  out_stream_ << boost::format("以%1%的价格购买%2%的%3%成功.")%price%quantity%instId << std::endl;
  return 0;
}

int market::FakeMarket::sell_market(const std::string &instId, const std::string &quantity,
               const std::string &price) {
  boost::multiprecision::cpp_dec_float_100 quantity_f(quantity);
  boost::multiprecision::cpp_dec_float_100 price_f(price);
  boost::multiprecision::cpp_dec_float_100 get_m = quantity_f * price_f;

  if (account_.count(instId) == 0 || account_[instId] < quantity_f) {
    out_stream_ << boost::format("以%1%的价格售出%2%的%3%失败，余额不足.")%price%quantity%instId << std::endl;
    return -1;
  }

  account_[instId] -= quantity_f;

  balance_ += get_m;

  out_stream_ << boost::format("以%1%的价格售出%2%的%3%成功.")%price%quantity%instId << std::endl;

  return 0;
}


boost::json::value market::FakeMarket::init() {
  boost::json::value config = {
    {"market", "okex"},
    {"out_file", "fake_market.out"},
    {"balance", "100"}
  };
  return config;
}