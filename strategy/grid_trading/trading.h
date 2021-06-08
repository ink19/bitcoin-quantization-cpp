#ifndef _STRATEGY_GRID_TRADING_TRADING_H
#define _STRATEGY_GRID_TRADING_TRADING_H

#include <boost/json.hpp>
#include <functional>
#include <sqlite/connection.hpp>
#include <tuple>
#include <memory>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <vector>

namespace grid_trading {

namespace mp = boost::multiprecision;

namespace trading_side {
  const int sell = 0;
  const int buy = 1;
}

struct balance_t {
  int trade_id;
  boost::multiprecision::cpp_dec_float_100 size;
  boost::multiprecision::cpp_dec_float_100 price;
  int step;
};

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
    if (step >= quantity_.size()) {
      return quantity_.back().str();
    } else {
      return quantity_[step].str();
    }
  };

  int quantity(std::vector<std::string> stable_quantity) {
    for (auto iquantity : stable_quantity) {
      quantity_.push_back(mp::cpp_dec_float_100(iquantity));
    }
    return 0;
  }
private:
  int insert_trading(const std::string & size, const std::string & price, const int step);
  int delete_trading(int id);
  int count_trading();
  // 对数据库中的交易进行同步
  int sync_trading();
  // 调整网格价格
  int adjust_grid();
  
  int buy_trading();
  int sell_trading();

  std::tuple<int, std::string, std::string> get_last_trading();

  template<typename T>
  T round_towards_zero(T v, int precision = -1) {
    if (precision == -1) precision = precision_;

    T scale = pow(T(10), precision);

    if (v.is_zero())
        return v;
    if (v < 0)
        return ceil(v*scale)/scale;
    else
        return floor(v*scale)/scale;
  }

  market_func_t trading_fun_;
  send_message_t send_message_fun_;
  std::unique_ptr<sqlite::connection> sql_conn_;

  mp::cpp_dec_float_100 grid_size_ = 0.02; // 百分之2
  int precision_ = 2;
  std::vector<mp::cpp_dec_float_100> quantity_ = {};
  std::vector<balance_t> balance_;
  mp::cpp_dec_float_100 last_price = -1;

  // 网格价格
  mp::cpp_dec_float_100 buy_price;
  mp::cpp_dec_float_100 upper_price;

  int step = 0;
};

}

#endif