#include "fake.h"
#include <boost/format.hpp>
#include <iostream>

market::FakeMarket::FakeMarket(boost::json::value config) {
  auto config_obj = config.as_object();
  std::string out_file_name = config_obj["out_file"].as_string().c_str();
  pocket_book_ = config_obj["pocket_book"].as_string().c_str();
  out_stream_.open(out_file_name, std::ios_base::app);
  read_pocket_book();
  // balance_ = boost::multiprecision::cpp_dec_float_100(balance_s);
}

int market::FakeMarket::buy_market(const std::string &instId, const std::string &quantity,
               const std::string &price) {
  boost::multiprecision::cpp_dec_float_100 quantity_f(quantity);
  boost::multiprecision::cpp_dec_float_100 price_f(price);
  boost::multiprecision::cpp_dec_float_100 require_m = quantity_f * price_f;

  if (require_m > balance_) {
    out_stream_ << boost::format("以%1%的价格购买%2%的%3%失败，余额不足，目前余额%4%.")%price%quantity%instId%balance_.str() << std::endl;
    return -1;
  }
  
  balance_ -= require_m;

  if (account_.count(instId) == 0) {
    account_[instId] = quantity_f * (1 + trading_fee_);
  } else {
    account_[instId] += quantity_f * (1 + trading_fee_);
  }
  write_pocket_book();
  out_stream_ << boost::format("以%1%的价格购买%2%的%3%成功，目前余额%4%.")%price%quantity%instId%balance_ << std::endl;
  return 0;
}

int market::FakeMarket::sell_market(const std::string &instId, const std::string &quantity,
               const std::string &price) {
  boost::multiprecision::cpp_dec_float_100 quantity_f(quantity);
  boost::multiprecision::cpp_dec_float_100 price_f(price);
  boost::multiprecision::cpp_dec_float_100 get_m = quantity_f * price_f;

  if (account_.count(instId) == 0 || account_[instId] < quantity_f) {
    out_stream_ << boost::format("以%1%的价格售出%2%的%3%失败，余额不足，目前余额%4%.")%price%quantity%instId%balance_.str() << std::endl;
    return -1;
  }

  account_[instId] -= quantity_f;

  balance_ += get_m * (1 + trading_fee_);

  write_pocket_book();
  out_stream_ << boost::format("以%1%的价格售出%2%的%3%成功，目前余额%4%.")%price%quantity%instId%balance_.str() << std::endl;

  return 0;
}


boost::json::value market::FakeMarket::init() {
  std::string pocket_book_filename = "fake_market_pocket_book.json";

  boost::json::value pocket_book_obj = {
    {"balance", "100"},
    {"account", boost::json::object()}
  };

  std::ofstream pocket_book_stream;
  pocket_book_stream.open(pocket_book_filename);
  pocket_book_stream << boost::json::serialize(pocket_book_obj);
  pocket_book_stream.close();

  boost::json::value config = {
    {"market", "okex"},
    {"out_file", "fake_market.out"},
    {"pocket_book", pocket_book_filename}
  };

  return config;
}

int market::FakeMarket::read_pocket_book() {
  std::ifstream pocket_book_stream;
  pocket_book_stream.open(pocket_book_);

  std::string out_data;
  pocket_book_stream >> out_data;

  auto pocket_book_obj = boost::json::parse(out_data).as_object();
  balance_ = mp::cpp_dec_float_100(pocket_book_obj["balance"].as_string().data());

  auto account_data = pocket_book_obj["account"].as_object();

  for (auto i_data : account_data) {
    account_[i_data.key_c_str()] = mp::cpp_dec_float_100(i_data.value().as_string().c_str());
  }

  return 0;
}

int market::FakeMarket::write_pocket_book() {
  std::ofstream pocket_book_stream;
  pocket_book_stream.open(pocket_book_);

  boost::json::object out_obj = {};
  out_obj["balance"] = balance_.str();
  auto out_acc = boost::json::object();

  for (auto iacc : account_) {
    out_acc[iacc.first] = iacc.second.str();
  }
  out_obj["account"] = out_acc;
  pocket_book_stream << boost::json::serialize(out_obj);
  return 0;
}