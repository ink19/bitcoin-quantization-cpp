#ifndef _MARKET_OKEX_OKEX_H
#define _MARKET_OKEX_OKEX_H

#include <string>
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ssl.hpp>
#include <memory>

namespace Okex {

namespace asio = boost::asio;
namespace http = boost::beast::http;
namespace ssl = boost::asio::ssl;

// POST请求类型
typedef http::request<http::string_body> post_request_t;
typedef std::shared_ptr<post_request_t> post_request_sp_t;

// GET请求类型
typedef http::request<http::empty_body> get_request_t;
typedef std::shared_ptr<get_request_t> get_request_sp_t;

class Okex {
public:
  Okex(asio::io_context &context);
private:
  post_request_sp_t generate_post_request_comm();
  post_request_sp_t generate_post_request(const std::string &request_path, const std::string &body);

  get_request_sp_t generate_get_request_comm();
  get_request_sp_t generate_get_request(const std::string &request_path);

  asio::io_context &context_;
  std::string api_key_;
  std::string secret_key_;
  std::string passphrase_;
  asio::ssl::context ssl_context = asio::ssl::context(ssl::context::tlsv12_client);
  const std::string host_name = "www.okex.com";
};

}

#endif