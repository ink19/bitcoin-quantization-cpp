#include "wework.h"
#include <boost/json.hpp>
#include <boost/format.hpp>
#include <iostream>

Wework::Wework::Wework(const KeyWords &, asio::io_context &context, const std::string &token, const std::string &key_words) : context_(context) {
  token_ = token;
  request_uri_ = "/cgi-bin/webhook/send?key=" + token;
  key_words_ = key_words;
}

int Wework::Wework::send_text(const std::string &level, const std::string &message) {
  std::string send_data = (boost::format("[%1%][%2%]%3%")%key_words_%level%message).str();
  std::string message_json_s = boost::json::serialize(boost::json::value({
    {"msgtype", "text"},
    {"text", {{"content", send_data}}}
  }));

  auto request = generate_post_request(message_json_s);
  // std::cout << *request << std::endl;

  std::string response_data = send_request(request);

  auto respone_obj = boost::json::parse(response_data).as_object();

  int result = respone_obj["errcode"].as_int64();

  return result;
}

Wework::post_request_sp_t Wework::Wework::generate_post_request(const std::string &data) {
  auto req_ = std::make_shared<post_request_t>();
  req_->version(11);
  req_->method(http::verb::post);
  req_->target(request_uri_);
  req_->set(http::field::content_type, "application/json");
  req_->set(http::field::host, host_name);
  req_->set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
  req_->body() = data;
  req_->prepare_payload();

  return req_;
}
