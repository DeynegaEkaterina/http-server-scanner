
#include "Server.h"
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <thread>
#include "Scanner.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

  int main() {
    try {
      auto const address = net::ip::make_address("0.0.0.0");
      auto const port = static_cast<uint16_t>(std::atoi("8080"));
      net::io_context ioc{1};
      tcp::acceptor acceptor{ioc, {address, port}};
      std::cout << "=====Scan server started====" << endl;

      for (;;) {
        tcp::socket socket{ioc};
        acceptor.accept(socket);
        std::thread{std::bind(&do_session, std::move(socket))}
            .detach();
      }
    } catch (std::exception &e) {
      std::cerr << e.what() << '\n';
      return EXIT_FAILURE;
    }
  }
