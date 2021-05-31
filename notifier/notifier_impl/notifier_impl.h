#ifndef _NOTIFIER_NOTIFIER_IMPL_H
#define _NOTIFIER_NOTIFIER_IMPL_H
#include <string>

namespace notifier {

class notifier_impl {
public:
  virtual ~notifier_impl(){};
  virtual int send_message(const std::string &level,
                           const std::string &message) {
    return 0;
  };
};

} // namespace notifier

#endif