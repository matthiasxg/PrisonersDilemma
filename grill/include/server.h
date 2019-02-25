#pragma once

#include <thread>
#include "src/includes.cpp"

class Server {
private:
    std::thread asioThread;
    void startAsioThread(int port);
public:
    Server(int port);
    ~Server();
};