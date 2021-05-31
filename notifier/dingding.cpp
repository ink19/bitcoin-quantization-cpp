#include "dingding.h"
#include <memory>
#include <boost/asio.hpp>
#include "dingding/dingding.h"

notifier::DingDingApi::DingDingApi(boost::asio::io_context &context, const std::string &type, const std::string &token, const std::string &key_words) {
  notifier_ = std::make_unique<DingDing::DingDing>(DingDing::KeyWords(), context, token, key_words);
}

notifier::DingDingApi::~DingDingApi(){}

int notifier::DingDingApi::send_message(const std::string& level, const std::string &message) {
  return notifier_->send_text(level, message);
}