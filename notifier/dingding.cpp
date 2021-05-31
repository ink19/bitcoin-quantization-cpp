#include "dingding.h"
#include <memory>
#include <boost/asio.hpp>
#include "dingding/dingding.h"
#include <boost/json.hpp>

notifier::DingDingApi::DingDingApi(boost::asio::io_context &context, boost::json::value config) {
  boost::json::object dingding_config_obj = config.as_object();
  std::string token = dingding_config_obj["token"].as_string().c_str();
  std::string key_words = dingding_config_obj["keywords"].as_string().c_str();
  notifier_ = std::make_unique<DingDing::DingDing>(DingDing::KeyWords(), context, token, key_words);
}

notifier::DingDingApi::~DingDingApi(){}

int notifier::DingDingApi::send_message(const std::string& level, const std::string &message) {
  return notifier_->send_text(level, message);
}

boost::json::value notifier::DingDingApi::init() {
  boost::json::value config = {
    {"token", ""},
    {"keywords", ""}
  };
  return config;
}