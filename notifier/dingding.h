#ifndef _NOTIFIER_DINGDING_H
#define _NOTIFIER_DINGDING_H

#include "notifier_impl/notifier_impl.h"
#include "dingding/dingding.h"
#include <memory>
#include <string>
#include <boost/asio.hpp>
#include <boost/json.hpp>

namespace notifier {

class DingDingApi : public notifier_impl {
public:
  DingDingApi(boost::asio::io_context &context, boost::json::value config);
  ~DingDingApi();
  int send_message(const std::string& level, const std::string &message);
  static boost::json::value init();

private:
  std::unique_ptr<DingDing::DingDing> notifier_;
};

}

#endif