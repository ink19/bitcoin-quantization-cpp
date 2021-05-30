#include "config.h"

#include <boost/json.hpp>
#include <fstream>
#include <iostream>

CommConfig::CommConfig(const std::string &filename) {
  boost::json::stream_parser json_parser;
  std::ifstream config_in;
  config_in.open(filename);
  std::string read_data;
  json_parser.reset();
  while (config_in >> read_data) {
    json_parser.write(read_data);
  }
  json_parser.finish();
  // json_parser.finish();
  config_in.close();

  config_json = json_parser.release();
}

int CommConfig::init(const std::string &filename) {
  boost::json::value json_data = {
    {
      "okex", {
        {"api_key", ""},
        {"secret_key", ""},
        {"passphrase", ""}
      }
    },
    {
      "dingding", {
        {"token", ""}
      }
    }, 
    {
      "net", {
        {"size", 0}
      }
    }
  };

  std::string config_str = boost::json::serialize(json_data);
  
  std::ofstream config_out;
  config_out.open(filename);
  
  config_out << config_str;

  return 0;
}