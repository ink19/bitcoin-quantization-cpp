#include "notifier.h"
#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <memory>
#include <iostream>
#include "dingding.h"
#include "wework.h"

notifier::Notifier::Notifier(boost::asio::io_context &context, boost::json::value &notifier_config) {
  boost::json::object notifier_config_obj = notifier_config.as_object();
  if (notifier_config_obj["notifier"] == "dingding") {
    notifier_ = std::make_unique<DingDingApi>(context, notifier_config_obj["dingding"]);
  } else if (notifier_config_obj["notifier"] == "wework") {
    notifier_ = std::make_unique<WeworkApi>(context, notifier_config_obj["wework"]);
  } else {
    std::cout << "Init Notifier Error." << std::endl;
    throw std::exception();
  }
}

boost::json::value notifier::Notifier::init() {
  boost::json::value notifier_config = {
    {"notifier", ""},
    {"dingding", DingDingApi::init()},
    {"wework", WeworkApi::init()}
  };
  return notifier_config;
}

notifier::Notifier::~Notifier() {}

int notifier::Notifier::send_message(const std::string& level, const std::string &message) {
  return notifier_->send_message(level, message);
}
