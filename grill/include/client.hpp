#pragma once

#include "logging_provider.hpp"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"
#include "asio.hpp"
#pragma GCC diagnostic pop

#include "PrisonersDilemma.pb.h"
#include "PrisonersDilemma.grpc.pb.h"
#include "player.hpp"

#include <string>
#include <vector>

class Client {
private:
    short unsigned int port;
    LoggingProvider logger = LoggingProvider::getInstance();
    void connectToServer(short unsigned int port);

    // Network
    void sendRequest(Player& client, Request& request);
    Response getResponse(Player& client);
public:
    Client(short unsigned int port);
    ~Client();
};