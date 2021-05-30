#ifndef _NOTIFIER_DINGDING_DINGDING_H
#define _NOTIFIER_DINGDING_DINGDING_H

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <map>
#include <memory>
#include <string>

namespace DingDing {

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



struct KeyWords{};

class DingDing {
public:
  DingDing(const KeyWords &, asio::io_context &context, const std::string &token, const std::string &key_words);
  int send_text(const std::string &level, const std::string &message);
private:
  post_request_sp_t generate_post_request(const std::string &data);
  template <typename T> std::string send_request(T request) {
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
    std::string str_res = res->body();
    // beast::error_code ec;
    // stream.shutdown();
    return str_res;
  }

  const std::string host_name = "oapi.dingtalk.com";
  const std::string port = "443";
  std::string token_ = "";
  std::string request_uri_ = "";
  std::string key_words_ = "";
  asio::io_context &context_;
};

};

#endif