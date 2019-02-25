#include "server.h"

using namespace std;
using namespace asio::ip;

Server::Server(int port) {
    //startAsioThread(port);
}

void Server::startAsioThread(int port) {
    spdlog::info("Prisoners dilemma server started!");

    asio::io_context ctx;
    tcp::endpoint ep{tcp::v4(), 3145};
    tcp::acceptor acceptor{ctx, ep};

    while (true)
    {
        spdlog::info("Just before accept");
        acceptor.listen();

        tcp::iostream strm{acceptor.accept()};

        if (strm)
        {
            strm << "lol";
            strm.close();
        }
        else
        {
            spdlog::error("Server unable to send data!");
        }

        spdlog::info("Closed connection");
    }

}