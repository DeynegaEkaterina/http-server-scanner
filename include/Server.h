//
// Created by ekaterina on 28.03.2021.
//
#ifndef SUGGESTION_SERVER_H
#define SUGGESTION_SERVER_H


#include "Scanner.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/config.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <shared_mutex>
#include <iomanip>
#include "filesystem"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;


template<class Body, class Allocator, class Send>
void handle_request(http::request<Body, http::basic_fields<Allocator>> &&req,
                    Send &&send) {

  auto const bad_request = [&req](beast::string_view why) {
    http::response<http::string_body> res{http::status::bad_request,
                                          req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = std::string(why);
    res.prepare_payload();
    return res;
  };

    std::string path_to_directory = req.body();
    std::cout << req.body() << std::endl;
    Scanner::printAnalytics(req.body());

    http::string_body::value_type body = req.body();
    auto const size = body.size();

  if (req.method() != http::verb::post && req.method() != http::verb::head) {
    return send(bad_request("Unknown HTTP-method"));
  }

  if (req.method() == http::verb::head) {
    http::response<http::empty_body> res{http::status::ok, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);

    res.content_length(size);
    res.keep_alive(req.keep_alive());
    return send(std::move(res));
  }

  http::response<http::string_body> res{
      std::piecewise_construct, std::make_tuple(std::move(body)),
      std::make_tuple(http::status::ok, req.version())};
  res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
 // res.content_length(size);
  res.keep_alive(req.keep_alive());
  res.keep_alive(req.keep_alive());
  return send(std::move(res));
}

void fail(beast::error_code ec, char const *what) {
  std::cerr << what << ": " << ec.message() << "\n";
}

template<class Stream>
struct send_lambda {
  Stream &stream_;
  bool &close_;
  beast::error_code &ec_;

  explicit send_lambda(Stream &stream, bool &close, beast::error_code &ec)
      : stream_(stream), close_(close), ec_(ec) {}

  template<bool isRequest, class Body, class Fields>
  void operator()(http::message<isRequest, Body, Fields> &&msg) const {
    close_ = msg.need_eof();


    http::serializer<isRequest, Body, Fields> sr{msg};
    http::write(stream_, sr, ec_);
  }
};


void do_session(net::ip::tcp::socket &socket) {

  bool close = false;
  beast::error_code ec;
  beast::flat_buffer buffer;
  send_lambda<tcp::socket> lambda{socket, close, ec};

  for (;;) {
    http::request<http::string_body> req;
    http::read(socket, buffer, req, ec);
    if (ec == http::error::end_of_stream) break;
    if (ec) return fail(ec, "read");
    handle_request(std::move(req), lambda);
    if (ec) return fail(ec, "write");
  }

  socket.shutdown(tcp::socket::shutdown_send, ec);
}

#endif  // SUGGESTION_SERVER_H
