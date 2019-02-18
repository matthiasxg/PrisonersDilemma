/*
 *  Autor: Matthias Grill
 *  Class: 5BHIF
 *  Date: 11.02.2019
 *  File: PrisonersDilemma_Server/main.cpp 
 *  ____                           
 * / ___|  ___ _ ____   _____ _ __ 
 * \___ \ / _ \ '__\ \ / / _ \ '__|
 *  ___) |  __/ |   \ V /  __/ |   
 * |____/ \___|_|    \_/ \___|_|   
 */
#include <iostream>
#include <thread>
#include "../includes.cpp"

using namespace std;
using namespace asio::ip;

//int main(int argc, char* argv[]) {
int main() {
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

    return 0;
}
