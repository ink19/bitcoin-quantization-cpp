#ifndef _NOTIFIER_WEWORK_H
#define _NOTIFIER_WEWORK_H

#include "notifier_impl/notifier_impl.h"
#include "wework/wework.h"
#include <memory>
#include <string>
#include <boost/asio.hpp>
#include <boost/json.hpp>

namespace notifier {

class WeworkApi : public notifier_impl {
public:
  WeworkApi(boost::asio::io_context &context, boost::json::value config);
  ~WeworkApi();
  int send_message(const std::string& level, const std::string &message);
  static boost::json::value init();

private:
  std::unique_ptr<Wework::Wework> notifier_;
};

}

#endif