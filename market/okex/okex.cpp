#include "okex.h"
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/json.hpp>
#include <chrono>
#include <cryptopp/base64.h>
#include <cryptopp/filters.h>
#include <cryptopp/hmac.h>
#include <cryptopp/sha.h>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <vector>

std::string hmac_sha256_base64(const std::string &data, const std::string &key) {
  // std::cout << data << std::endl;
  CryptoPP::HMAC<CryptoPP::SHA256> hmac((CryptoPP::byte *)key.c_str(),
                                        key.length());
  std::string mac, encoded;

  CryptoPP::StringSource hmac_source(
      data, true,
      new CryptoPP::HashFilter(hmac,
                               new CryptoPP::StringSink(mac)) // HashFilter
  );                                                          // StringSource

  encoded.clear();
  CryptoPP::StringSource base64_source(
      mac, true,
      new CryptoPP::Base64Encoder(
          new CryptoPP::StringSink(encoded)) // Base64Encoder
  );                                         // StringSource

  return encoded.substr(0, encoded.size() - 1);
}

std::string get_iso_time() {
  boost::posix_time::ptime t =
      boost::posix_time::microsec_clock::universal_time();
  auto temp_data = boost::posix_time::to_iso_extended_string(t);
  temp_data = temp_data.substr(0, temp_data.size() - 3);
  return temp_data + "Z";
}

Okex::Okex::Okex(asio::io_context &context, const std::string &api_key,
                 const std::string &secret_key, const std::string &passphrase)
    : context_(context) {
  api_key_ = api_key;
  secret_key_ = secret_key;
  passphrase_ = passphrase;
}

Okex::post_request_sp_t
Okex::Okex::generate_post_request(const std::string &request_path,
                                  const std::string &body) {
  // std::cout << body << std::endl;
  // std::cout << request_path << std::endl;
  auto req_ = generate_post_request_comm();
  req_->target(request_path);

  std::string timestamp = get_iso_time();
  std::string method = "POST";
  std::string encode_data = timestamp + method + request_path + body;

  req_->set("OK-ACCESS-TIMESTAMP", timestamp);
  req_->set("OK-ACCESS-SIGN", hmac_sha256_base64(encode_data, secret_key_));
  // req_->set(http::field::content_length, boost::lexical_cast<std::string>(body.size()));
  req_->body() = body;
  req_->prepare_payload();

  // std::cout << *req_ << std::endl;
  return req_;
}

Okex::get_request_sp_t
Okex::Okex::generate_get_request(const std::string &request_path) {
  auto req_ = generate_get_request_comm();
  req_->target(request_path);

  std::string timestamp = get_iso_time();
  std::string method = "GET";
  std::string encode_data = timestamp + method + request_path;

  req_->set("OK-ACCESS-TIMESTAMP", timestamp);
  req_->set("OK-ACCESS-SIGN", hmac_sha256_base64(encode_data, secret_key_));
  // std::cout << timestamp << std::endl;
  return req_;
}

Okex::post_request_sp_t Okex::Okex::generate_post_request_comm() {
  auto req_ = std::make_shared<post_request_t>();
  req_->version(11);
  req_->method(http::verb::post);
  req_->set(http::field::content_type, "application/json");
  req_->set(http::field::host, host_name);
  req_->set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
  
  req_->set("OK-ACCESS-KEY", api_key_);
  req_->set("OK-ACCESS-PASSPHRASE", passphrase_);
  if (simulated_trading_) {
    req_->set("x-simulated-trading", "1");
  }
  return req_;
}

Okex::get_request_sp_t Okex::Okex::generate_get_request_comm() {
  auto req_ = std::make_shared<get_request_t>();
  req_->version(11);
  req_->method(http::verb::get);
  req_->set(http::field::host, host_name);
  req_->set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

  req_->set("OK-ACCESS-KEY", api_key_);
  req_->set("OK-ACCESS-PASSPHRASE", passphrase_);
  if (simulated_trading_) {
    req_->set("x-simulated-trading", "1");
  }
  return req_;
}

Okex::ss_map_sp_t
Okex::Okex::account_balance(const std::vector<std::string> &ccy) {
  std::string query_uri = "/api/v5/account/balance";
  ss_map_sp_t result = std::make_shared<ss_map_t>();

  if (ccy.size() != 0) {
    query_uri += "?ccy=";
    query_uri += boost::join(ccy, ",");
  }

  auto request = generate_get_request(query_uri);

  auto respone_body = send_request(request);

  auto json_obj = boost::json::parse(respone_body).as_object();

  if (json_obj["code"].as_string() != "0") {
    std::cout << json_obj["msg"].as_string() << std::endl;
    throw std::exception();
  }

  for (auto item :
       json_obj["data"].as_array()[0].as_object()["details"].as_array()) {
    auto item_obj = item.as_object();
    (*result)[std::string(item_obj["ccy"].as_string().data())] =
        item_obj["cashBal"].as_string().data();
  }

  return result;
}

Okex::ss_map_sp_t Okex::Okex::market_ticket(const std::string &instId) {
  ss_map_sp_t result = std::make_shared<ss_map_t>();
  std::string query_uri = "/api/v5/market/ticker";
  query_uri += "?instId=" + instId;

  auto request = generate_get_request(query_uri);
  auto respone_obj = boost::json::parse(send_request(request)).as_object();

  if (respone_obj["code"].as_string() != "0") {
    std::cout << respone_obj["msg"].as_string() << std::endl;
    throw std::exception();
  }

  for (auto item : respone_obj["data"].as_array()[0].as_object()) {
    (*result)[std::string(item.key().data())] =
        std::string(item.value().as_string().data());
  }

  return result;
}

int Okex::Okex::set_simulated_trading() {
  simulated_trading_ = true;
  return 0;
}

Okex::ss_map_sp_t Okex::Okex::trade_order(const std::string &instId, const std::string &tdMode,
  const std::string &side, const std::string &ordType,
  const std::string &sz, const std::string &px) {
  ss_map_sp_t result = std::make_shared<ss_map_t>();
  std::string query_uri = "/api/v5/trade/order";
  
  boost::json::value json_data = {
    {"instId", instId},
    {"tdMode", tdMode},
    {"side", side},
    {"ordType", ordType},
    {"sz", sz},
    {"px", px}
  };

  auto request = generate_post_request(query_uri, boost::json::serialize(json_data));

  auto respone_obj = boost::json::parse(send_request(request)).as_object();

  (*result)["code"] = std::string(respone_obj["code"].as_string().data());

  if (respone_obj["code"].as_string() != "0") {
    return result;
  }

  for (auto item : respone_obj["data"].as_array()[0].as_object()) {
    (*result)[std::string(item.key().data())] =
        std::string(item.value().as_string().data());
  }

  return result;
}