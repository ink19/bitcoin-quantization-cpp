#include "notifier.h"
#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <memory>
#include <iostream>

notifier::Notifier::Notifier(boost::asio::io_context &context, boost::json::value &notifier_config) {
  boost::json::object notifier_config_obj = notifier_config.as_object();
  if (notifier_config_obj["notifier"] == "dingding") {
    boost::json::object dingding_config_obj = notifier_config_obj["dingding"].as_object();
    std::string dingding_token = dingding_config_obj["token"].as_string().c_str();
    std::string dingding_keywords = dingding_config_obj["keywords"].as_string().c_str();
    notifier_ = std::make_unique<DingDingApi>(context, "keywords", dingding_token, dingding_keywords);
  } else {
    std::cout << "Init Notifier Error." << std::endl;
    throw std::exception();
  }
}

boost::json::value notifier::Notifier::init() {
  boost::json::value notifier_config = {
    {"notifier", ""},
    {"dingding", {
      {"token", ""},
      {"keywords", ""}
    }}
  };
  return notifier_config;
}

notifier::Notifier::~Notifier() {}

int notifier::Notifier::send_message(const std::string& level, const std::string &message) {
  return notifier_->send_message(level, message);
}
