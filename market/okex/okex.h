#ifndef _MARKET_OKEX_OKEX_H
#define _MARKET_OKEX_OKEX_H

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <map>
#include <memory>
#include <string>

namespace Okex {
typedef unsigned char byte;
namespace asio = boost::asio;
namespace http = boost::beast::http;
namespace ssl = boost::asio::ssl;
namespace beast = boost::beast;
// POST请求类型
typedef http::request<http::string_body> post_request_t;
typedef std::shared_ptr<post_request_t> post_request_sp_t;

// GET请求类型
typedef http::request<http::empty_body> get_request_t;
typedef std::shared_ptr<get_request_t> get_request_sp_t;

typedef http::response<http::string_body> response_t;
typedef std::shared_ptr<response_t> response_sp_t;

typedef std::map<std::string, std::string> ss_map_t;
typedef std::shared_ptr<ss_map_t> ss_map_sp_t;


namespace trade_order {
  namespace tdMode {
    const std::string cash = "cash";
    const std::string isolated = "isolated";
    const std::string cross = "cross";
  };

  namespace posSide {
    const std::string buy = "buy";
    const std::string sell = "sell";
    const std::string _long = "long";
    const std::string _short = "short";
  }

  namespace ordType {
    // 普通委托
    // 限价单，要求指定sz 和 px
    const std::string limit = "limit";
    // 市价单
    const std::string market = "market";
    
    // 高级委托
    // 限价委托，Post-only在下单那一刻只做maker，如果该笔订单的任何部分会吃掉当前挂单深度，则该订单将被全部撤销。
    const std::string post_only = "post_only";
    // 限价委托，全部成交或立即取消，如果无法全部成交该笔订单，则该订单将被全部撤销。
    const std::string fok = "fok";
    // 限价委托，立即成交并取消剩余，立即按照委托价格成交，并取消该订单剩余未完成数量，不会再深度列表上展示委托数量。
    const std::string ioc = "ioc";
    // 市价委托，立即成交并取消剩余，仅适用于交割合约和永续合约。 
    const std::string optimal_limit_ioc = "optimal_limit_ioc";
  }
}

class Okex {
public:
  Okex(asio::io_context &context, const std::string &api_key,
       const std::string &secret_key, const std::string &passphrase);
  int set_simulated_trading();
  ss_map_sp_t account_balance(const std::vector<std::string> &ccy = {});
  ss_map_sp_t market_ticket(const std::string &instId);
  ss_map_sp_t trade_order(const std::string &instId, const std::string &tdMode,
                          const std::string &side, const std::string &ordType,
                          const std::string &sz, const std::string &px);
  ss_map_sp_t get_trading_fee(const std::string &instId);
private:
  post_request_sp_t generate_post_request_comm();
  post_request_sp_t generate_post_request(const std::string &request_path,
                                          const std::string &body);

  get_request_sp_t generate_get_request_comm();
  get_request_sp_t generate_get_request(const std::string &request_path);

  template <typename T> std::string send_request(T request, int try_time = 0) {
    std::string str_res = "";
    try {
      ssl::context ctx(ssl::context::tls);

      ctx.set_default_verify_paths();
      // ctx.set_verify_mode(ssl::context::verify_peer);

      asio::ip::tcp::resolver resolver(context_);

      // ssl 流
      beast::ssl_stream<beast::tcp_stream> stream(context_, ctx);

      if (!SSL_set_tlsext_host_name(stream.native_handle(), host_name.c_str())) {
        beast::error_code ec{static_cast<int>(::ERR_get_error()),
                            asio::error::get_ssl_category()};
        throw beast::system_error{ec};
      }

      auto const results = resolver.resolve(host_name, port);
      beast::get_lowest_layer(stream).connect(results);

      stream.handshake(ssl::stream_base::client);

      http::write(stream, *request);
      response_sp_t res = std::make_shared<response_t>();
      beast::flat_buffer _buffer;
      http::read(stream, _buffer, *res);
      str_res = res->body();
    } catch (std::exception e) {
      if (try_time < 10) {
        return send_request(request, try_time + 1);
      } else {
        throw e;
      }
    }
    
    // beast::error_code ec;
    // stream.shutdown();
    return str_res;
  }

  bool simulated_trading_ = false;
  asio::io_context &context_;
  std::string api_key_;
  std::string secret_key_;
  std::string passphrase_;
  const std::string host_name = "www.okex.com";
  const std::string port = "443";
};

} // namespace Okex

#endif