#include "wework.h"
#include <memory>
#include <boost/asio.hpp>
#include "wework/wework.h"
#include <boost/json.hpp>

notifier::WeworkApi::WeworkApi(boost::asio::io_context &context, boost::json::value config) {
  boost::json::object wework_config_obj = config.as_object();
  std::string token = wework_config_obj["token"].as_string().c_str();
  std::string key_words = wework_config_obj["keywords"].as_string().c_str();
  notifier_ = std::make_unique<Wework::Wework>(Wework::KeyWords(), context, token, key_words);
}

notifier::WeworkApi::~WeworkApi(){}

int notifier::WeworkApi::send_message(const std::string& level, const std::string &message) {
  return notifier_->send_text(level, message);
}

boost::json::value notifier::WeworkApi::init() {
  boost::json::value config = {
    {"token", ""},
    {"keywords", ""}
  };
  return config;
}