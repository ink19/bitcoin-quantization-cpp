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



int grid_trading::trading::sync_trading() {
  balance_.clear();
  sqlite::query get_q(*sql_conn_, "SELECT * FROM trading ORDER BY id;");
  boost::shared_ptr<sqlite::result> sql_res = get_q.get_result();
  std::tuple<int, std::string, std::string> result;
  // sql_res->next_row();
  // result = {sql_res->get_int(0), sql_res->get_string(1), sql_res->get_string(2)};
  while (sql_res->next_row()) {
    balance_.push_back({
      sql_res->get_int(0),
      boost::multiprecision::cpp_dec_float_100(sql_res->get_string(1)),
      boost::multiprecision::cpp_dec_float_100(sql_res->get_string(2)),
      sql_res->get_int(3)
    });
  }
  
  if (balance_.size() != 0) {
    step = balance_.back().step;
  }
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
  sqlite::execute temp_exec(con, "CREATE TABLE trading (id INTEGER PRIMARY KEY AUTOINCREMENT, size TEXT not null, price TEXT not null, step INTEGER not null);", true);
  // temp_exec();
  
  return 0;
}

grid_trading::trading::trading(const std::string &filename) {
  trading_fun_ = [](int, const std::string &, const std::string &){};
  send_message_fun_ = [](const std::string &, const std::string &){};
  sql_conn_ = std::make_unique<sqlite::connection>(filename);
  sync_trading();
}

int grid_trading::trading::buy_trading() {
  int m_step = step;
  if (m_step >= quantity_.size()) m_step = quantity_.size() - 1;
  mp::cpp_dec_float_100 buy_size = quantity_[m_step];
  buy_size = round_towards_zero(buy_size, 5);

  trading_fun_(trading_side::buy, buy_size.str(), buy_price.str());

  insert_trading(buy_size.str(), buy_price.str());
  auto last_trading = get_last_trading();

  balance_.push_back({
    std::get<0>(last_trading),
    buy_size,
    buy_price,
    m_step
  });
  step++;

  send_message_fun_("Warning", (boost::format("以%1%的价格买入%2%")%buy_price.str()%buy_size.str()).str());

  return 0;
}

int grid_trading::trading::commit_price(const std::string &price) {
  mp::cpp_dec_float_100 cpp_price(price);
  
  if (last_price == -1) {
    last_price = cpp_price;
    adjust_grid();
  } else {
    last_price = cpp_price;
  }

  if (buy_price >= last_price) {
    buy_trading();
    adjust_grid();
  } 

  if (last_price >= upper_price) {
    adjust_grid();
  }


  while (balance_.size() != 0) {
    mp::cpp_dec_float_100 sell_price = balance_.back().price * (1 + grid_size_);
    sell_price = round_towards_zero(buy_price);
    if (last_price >= sell_price) {
      sell_trading();
    } else {
      break;
    }
  }

  return 0;
}

int grid_trading::trading::sell_trading() {
  if (balance_.size() == 0) return -1;
  auto sell_goods = balance_.back();
  mp::cpp_dec_float_100 sell_price = sell_goods.price * (1 + grid_size_);
  sell_price = round_towards_zero(buy_price);
  
  trading_fun_(trading_side::sell, sell_goods.size.str(), sell_price.str());
  
  delete_trading(sell_goods.trade_id);
  balance_.pop_back();
  send_message_fun_("Warning", (boost::format("以%1%的价格买出%2%")%sell_price.str()%sell_goods.size.str()).str());
  if (step != 0) step--;
  return 0;
}

int grid_trading::trading::grid_size(const std::string &size) {
  grid_size_ = mp::cpp_dec_float_100(size);

  if (last_price != -1) {
    adjust_grid();
  }
  return 0;
}

int grid_trading::trading::adjust_grid() {
  if (last_price == -1) return 0;
  buy_price = last_price * (1 - grid_size_);
  buy_price = round_towards_zero(buy_price);
  upper_price = last_price * (1 + grid_size_);
  upper_price = round_towards_zero(upper_price);

  mp::cpp_dec_float_100 sell_price;
  if (balance_.size() == 0) {
    sell_price = -1;
  } else {
    sell_price = balance_.back().price * (1 + grid_size_);
  }

  send_message_fun_("Warning", (boost::format("修改网格价格买入价格为%1%，上界价格为%2%，卖出价格为%3%")%buy_price.str()%upper_price.str()%sell_price.str()).str());
  return 0;
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
  sqlite::query get_q(*sql_conn_, "SELECT * FROM trading ORDER BY id DESC LIMIT 1;");
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