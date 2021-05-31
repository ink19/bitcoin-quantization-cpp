#include "trading.h"
#include <sqlite/connection.hpp>
#include <sqlite/execute.hpp>
#include <stdio.h>
#include <unistd.h>
#include <memory>
#include <sqlite/database_exception.hpp>
#include <iostream>
#include <sqlite/query.hpp>
#include <sqlite/result.hpp>
#include <boost/format.hpp>

template <int decimals = 0, typename T>
T round_towards_zero(T const& v)
{
    static const T scale = pow(T(10), decimals);

    if (v.is_zero())
        return v;

    // ceil/floor is found via ADL and uses expression templates for optimization
    if (v<0)
        return ceil(v*scale)/scale;
    else
        // floor is found via ADL and uses expression templates for optimization
        return floor(v*scale)/scale;
}

int grid_trading::trading::commit_price(const std::string &price) {
  mp::cpp_dec_float_100 now_price(price);
  if (last_price == -1) {
    last_price = now_price;
    mp::cpp_dec_float_100 unpre_buy_price = last_price * (1 - grid_size_);
    buy_price = round_towards_zero<2>(unpre_buy_price);
    mp::cpp_dec_float_100 unpre_top_price = last_price * (1 + grid_size_);
    sell_price = round_towards_zero<2>(unpre_top_price);
  } else {
    last_price = now_price;
  }
  
  if (now_price >= sell_price && step > 0) {
    // 卖出
    trading_fun_(trading_side::sell, sell_size.str(), sell_price.str());
    send_message_fun_("Warning", (boost::format("以%1%的价格卖出%2%")%sell_price%sell_size).str());
    delete_trading(last_stack_id);
    step = count_trading();
    if (step > 0) {
      get_next_sell();
      send_message_fun_("Warning", (boost::format("修改网格价格，补仓价格%1%，出仓价格%2%，网格上界%3%")%buy_price%sell_price%top_price).str());
    } 
  }

  if (now_price > top_price) {
    mp::cpp_dec_float_100 unpre_buy_price = now_price * (1 - grid_size_);
    buy_price = round_towards_zero<2>(unpre_buy_price);
    mp::cpp_dec_float_100 unpre_top_price = now_price * (1 + grid_size_);
    sell_price = round_towards_zero<2>(unpre_top_price);
    send_message_fun_("Warning", (boost::format("修改网格价格，补仓价格%1%，出仓价格%2%，网格上界%3%")%buy_price%sell_price%top_price).str());
  }

  if (now_price <= buy_price) {
    mp::cpp_dec_float_100 buy_size = quantity_/now_price;
    trading_fun_(trading_side::buy, buy_size.str(), buy_price.str());
    insert_trading(buy_size.str(), buy_price.str());
    get_next_sell();

    mp::cpp_dec_float_100 unpre_buy_price = now_price * (1 - grid_size_);
    buy_price = round_towards_zero<2>(unpre_buy_price);
    mp::cpp_dec_float_100 unpre_top_price = now_price * (1 + grid_size_);
    sell_price = round_towards_zero<2>(unpre_top_price);

    send_message_fun_("Warning", (boost::format("修改网格价格，补仓价格%1%，出仓价格%2%，网格上界%3%")%buy_price%sell_price%top_price).str());
  }

  return 0;
}

int grid_trading::trading::grid_size(const std::string &size) {
  grid_size_ = mp::cpp_dec_float_100(size);
  if (last_price == -1) return 0;

  mp::cpp_dec_float_100 unpre_buy_price = last_price * (1 - grid_size_);
  buy_price = round_towards_zero<2>(unpre_buy_price);
  mp::cpp_dec_float_100 unpre_top_price = last_price * (1 + grid_size_);
  sell_price = round_towards_zero<2>(unpre_top_price);
  return 0;
}

int grid_trading::trading::get_next_sell() {
  auto last_trading = get_last_trading();
  last_stack_id = std::get<0>(last_trading);
  sell_size = mp::cpp_dec_float_100(std::get<1>(last_trading));
  sell_price = mp::cpp_dec_float_100(std::get<2>(last_trading)) * (1 + grid_size_);
  return 0;
}

int grid_trading::trading::count_trading() {
  sqlite::query get_q(*sql_conn_, "SELECT COUNT(*) FROM trading;");
  boost::shared_ptr<sqlite::result> sql_res = get_q.get_result();
  std::tuple<int, std::string, std::string> result;
  sql_res->next_row();

  return sql_res->get_int(0);
}

int grid_trading::trading::init(const std::string &filename) {
  if (access(filename.c_str(), F_OK) == 0) {
    remove(filename.c_str());
  }
  
  // 创建表
  sqlite::connection con(filename);
  sqlite::execute temp_exec(con, "CREATE TABLE trading (id INTEGER PRIMARY KEY AUTOINCREMENT, size TEXT not null, price TEXT not null);", true);
  // temp_exec();
  
  return 0;
}

grid_trading::trading::trading(const std::string &filename) {
  trading_fun_ = market_func_t();
  send_message_fun_ = send_message_t();
  sql_conn_ = std::make_unique<sqlite::connection>(filename);
  step = count_trading();
  if (step != 0) {
    get_next_sell();
    last_price = sell_price / (1 + grid_size_);
    last_price = round_towards_zero<2>(last_price);
    mp::cpp_dec_float_100 unpre_buy_price = last_price * (1 - grid_size_);
    buy_price = round_towards_zero<2>(unpre_buy_price);
    mp::cpp_dec_float_100 unpre_top_price = last_price * (1 + grid_size_);
    sell_price = round_towards_zero<2>(unpre_top_price);
  } else {
    last_price = -1;
  }
}

grid_trading::trading::~trading() { 
}

int grid_trading::trading::insert_trading(const std::string & size, const std::string & price) {
  sqlite::execute ins(*sql_conn_, "INSERT INTO trading (size, price) VALUES (?, ?);");
  ins % size % price;
  try {
    ins();
  } catch (sqlite::database_exception e) {
    std::cout << e.what() << std::endl;
    return -1;
  }
  return 0;
}

std::tuple<int, std::string, std::string> grid_trading::trading::get_last_trading() {
  sqlite::query get_q(*sql_conn_, "SELECT * FROM trading ORDERY BY id DESC LIMIT 1;");
  boost::shared_ptr<sqlite::result> sql_res = get_q.get_result();
  std::tuple<int, std::string, std::string> result;
  sql_res->next_row();
  result = {sql_res->get_int(0), sql_res->get_string(1), sql_res->get_string(2)};

  return result;
}

int grid_trading::trading::delete_trading(int id) {
  sqlite::execute del_e(*sql_conn_, "DELETE FROM trading WHERE id = ?");
  del_e % id;

  try {
    del_e();
  } catch (sqlite::database_exception e) {
    std::cout << e.what() << std::endl;
    return -1;
  }
  return 0;
}