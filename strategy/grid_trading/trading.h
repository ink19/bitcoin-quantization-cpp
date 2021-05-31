#ifndef _STRATEGY_GRID_TRADING_TRADING_H
#define _STRATEGY_GRID_TRADING_TRADING_H

#include <boost/json.hpp>
#include <functional>
#include <sqlite/connection.hpp>
#include <tuple>
#include <memory>
#include <boost/multiprecision/cpp_dec_float.hpp>

namespace grid_trading {

namespace mp = boost::multiprecision;

namespace trading_side {
  const int sell = 0;
  const int buy = 0;
}

typedef std::function<void(int, const std::string&, const std::string&)> market_func_t;
typedef std::function<void(const std::string&, const std::string&)> send_message_t;

class trading {
public:
  trading(const std::string &filename = "grid_trading.db");
  ~trading();
  static int init(const std::string &filename = "grid_trading.db");
  int commit_price(const std::string &price);
  int set_trading_fun(market_func_t trading_fun) {
    trading_fun_ = trading_fun;
    return 0;
  }
  int set_send_message_fun(send_message_t send_message_fun) {
    send_message_fun_ = send_message_fun;
    return 0;
  }

  std::string grid_size() {
    return grid_size_.str(2, std::ios_base::fixed);
  };

  int precision() {
    return precision_;
  }

  int grid_size(const std::string &size);

  int precision(int pre) {
    precision_ = pre;
    return 0;
  }
  
  std::string quantity() {
    return quantity_.str();
  };

  int quantity(std::string stable_quantity) {
    quantity_ = mp::cpp_dec_float_100(stable_quantity);
    return 0;
  }
private:
  int insert_trading(const std::string & size, const std::string & price);
  int delete_trading(int id);
  int count_trading();
  int get_next_sell();
  std::tuple<int, std::string, std::string> get_last_trading();
  
  market_func_t trading_fun_;
  send_message_t send_message_fun_;
  std::unique_ptr<sqlite::connection> sql_conn_;

  mp::cpp_dec_float_100 grid_size_ = 0.02; // 百分之2
  int precision_ = 2;
  mp::cpp_dec_float_100 quantity_ = 10;

  // 网格信息
  mp::cpp_dec_float_100 buy_price;
  mp::cpp_dec_float_100 top_price;
  mp::cpp_dec_float_100 last_price;

  mp::cpp_dec_float_100 sell_price;
  mp::cpp_dec_float_100 sell_size;

  int step = 0;
  int last_stack_id = 0;
};

}

#endif