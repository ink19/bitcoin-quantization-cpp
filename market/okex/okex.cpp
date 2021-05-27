#include "okex.h"
#include <cryptopp/base64.h>
#include <cryptopp/filters.h>
#include <cryptopp/hmac.h>
#include <cryptopp/sha.h>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>

std::string hmac_sha256_base64(std::string &data, std::string &key) {
  CryptoPP::HMAC<CryptoPP::SHA256> hmac((CryptoPP::byte *)key.c_str(), key.length());
  std::string mac, encoded;

  CryptoPP::StringSource hmac_source(
      data, true,
      new CryptoPP::HashFilter(hmac,
                               new CryptoPP::StringSink(mac)) // HashFilter
  );                                                          // StringSource

  encoded.clear();
  CryptoPP::StringSource base64_source(mac, true,
                         new CryptoPP::Base64Encoder(
                             new CryptoPP::StringSink(encoded)) // Base64Encoder
  );                                                            // StringSource

  return encoded;
}

std::string get_iso_time() {
  auto now = std::time(nullptr);
  std::string timestamp = std::put_time(std::localtime(&now), "%F %T%z")._M_fmt;
  return timestamp;
}

Okex::Okex::Okex(asio::io_context &context) : context_(context) {}

Okex::post_request_sp_t
Okex::Okex::generate_post_request(const std::string &request_path,
                                  const std::string &body) {
  auto req_ = generate_post_request_comm();
  req_->target(request_path);

  std::string timestamp = get_iso_time();
  std::string method = "GET";
  std::string encode_data = timestamp + method + request_path + body;
  
  req_->set("OK-ACCESS-SIGN", hmac_sha256_base64(encode_data, secret_key_));
  req_->set("OK-ACCESS-TIMESTAMP", timestamp);

  return req_;
}

Okex::post_request_sp_t Okex::Okex::generate_post_request_comm() {
  auto req_ = std::make_shared<post_request_t>();
  req_->version(11);
  req_->method(http::verb::post);
  req_->set(http::field::host, host_name);
  req_->set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
  req_->set("OK-ACCESS-KEY", api_key_);
  req_->set("OK-ACCESS-PASSPHRASE", passphrase_);

  return req_;
}

Okex::get_request_sp_t
Okex::Okex::generate_get_request(const std::string &request_path) {
  auto req_ = generate_get_request_comm();
  req_->target(request_path);
  
  std::string timestamp = get_iso_time();
  std::string method = "GET";
  std::string encode_data = timestamp + method + request_path;
  
  req_->set("OK-ACCESS-SIGN", hmac_sha256_base64(encode_data, secret_key_));
  req_->set("OK-ACCESS-TIMESTAMP", timestamp);

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
  return req_;
}