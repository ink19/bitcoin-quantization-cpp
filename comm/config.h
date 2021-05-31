#ifndef _COMM_CONFIG_H
#define _COMM_CONFIG_H

#include <string>
#include <boost/json.hpp>

// 专门用来读取配置文件，实现的不够优雅
class CommConfig {
public:
  CommConfig(boost::json::value config);
  ~CommConfig();
  static boost::json::value init();

  std::string trading_pair;
  double refresh_interval;

};

#endif