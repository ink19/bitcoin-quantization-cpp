#ifndef _NOTIFIER_NOTIFIER_H
#define _NOTIFIER_NOTIFIER_H

#include "notifier_impl/notifier_impl.h"
#include "dingding.h"
#include <memory>
#include <boost/json.hpp>
#include <boost/asio.hpp>

namespace notifier {

class Notifier : public notifier_impl {
public:
  Notifier(boost::asio::io_context &context, boost::json::value &notifier_config);
  static boost::json::value init();
  ~Notifier();
  int send_message(const std::string& level, const std::string &message);

private:
  std::unique_ptr<notifier_impl> notifier_;
};

}

#endif