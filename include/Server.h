//
// Created by ekaterina on 05.07.21
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
#include <string>
#include "filesystem"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;


template<class Body, class Allocator, class Send>
void handle_request(http::request<Body, http::basic_fields<Allocator>> &&req,
                    Send &&send) {
    std::string path_to_directory = req.body();
    std::cout << req.body() << std::endl;
    string output = Scanner::printAnalytics(req.body());
    http::response<http::string_body> res{http::status::bad_request,
                                          req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = output;
    res.prepare_payload();
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
