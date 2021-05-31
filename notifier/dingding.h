#ifndef _NOTIFIER_DINGDING_H
#define _NOTIFIER_DINGDING_H

#include "notifier_impl/notifier_impl.h"
#include "dingding/dingding.h"
#include <memory>
#include <string>
#include <boost/asio.hpp>

namespace notifier {

class DingDingApi : public notifier_impl {
public:
  DingDingApi(boost::asio::io_context &context, const std::string &type, const std::string &token, const std::string &key_words);
  ~DingDingApi();
  int send_message(const std::string& level, const std::string &message);

private:
  std::unique_ptr<DingDing::DingDing> notifier_;
};

}

#endif