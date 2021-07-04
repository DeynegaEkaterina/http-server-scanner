#include <Suggestions.hpp>
#include "Server.h"
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <thread>


namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

  int main(int argc, char* argv[]) {
    try {
      if (argc != 3) {
        std::cerr << "Usage: suggestion_server <address> <port>\n"
                  << "Example:\n"
                  << "    http-server-sync 0.0.0.0 8080\n";
        return EXIT_FAILURE;
      }
      auto const address = net::ip::make_address(argv[1]);
      auto const port = static_cast<uint16_t>(std::atoi(argv[2]));

      //отвечает за ввод вывод в консоль( 1 поток)
      net::io_context ioc{1};

      tcp::acceptor acceptor{ioc, {address, port}};

      std::thread{suggestion_updater}.detach();
      // detach - отделяет поток выполнения от объекта потока, позволяя
      // выполнению продолжаться независимо. Все выделенные ресурсы будут
      // освобождены после выхода потока.
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

  //./demo 0.0.0.0 8080
  // curl -d '{"input":"cou"}' -H "Content-Type: application/json" -X POST