#include <iostream>
#include <boost/program_options.hpp>
#include "utils.h"

int main(int argc, char *args[]) {
  boost::program_options::options_description cmd_parser("BitCoin Quantization 0.0.1");
  cmd_parser.add_options()("help,h", "打印帮助文件");
  cmd_parser.add_options()("init", "初始化");
  cmd_parser.add_options()("config,c", boost::program_options::value<std::string>()->default_value("config.json")->value_name("file"),"配置文件");

  boost::program_options::variables_map var_map;
  
  try {
    boost::program_options::store(boost::program_options::parse_command_line(argc, args, cmd_parser), var_map);
  } catch (boost::program_options::error e) {
    std::cout << e.what() << std::endl;
    std::cout << cmd_parser << std::endl;
    return 0;
  }

  if (var_map.count("help") != 0) {
    std::cout << cmd_parser << std::endl;
    return 0;
  }

  std::string config_file = var_map["config"].as<std::string>();

  if (var_map.count("init") != 0) {
    // std::cout << cmd_parser << std::endl;
    Utils::init(config_file);
    return 0;
  }
  
  Utils util(config_file);
  util.run();

  return 0;
}
