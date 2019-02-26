#pragma once
#include "logging_provider.hpp"
#include "PrisonersDilemma.pb.h"
#include "PrisonersDilemma.grpc.pb.h"
#include "player.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"
#include "asio.hpp"
#pragma GCC diagnostic pop

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

class Server {
private:
    short unsigned int port;
    LoggingProvider logger = LoggingProvider::getInstance();
    std::vector<Player> clients;
    void startServer();

    void handleClient(Player client);

    // Network
    void sendResponse(Player& client, Response& response);
    Request getRequest(Player& client);
public:
    Server(short unsigned int port);
    ~Server();
};