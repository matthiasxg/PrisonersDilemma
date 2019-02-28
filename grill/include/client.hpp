#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"
#include "asio.hpp"
#pragma GCC diagnostic pop

#include "PrisonersDilemma.pb.h"
#include "PrisonersDilemma.grpc.pb.h"
#include "player.hpp"
#include "logging_provider.hpp"
#include "json.hpp"
#include "strategy.hpp"

#include <string>
#include <vector>
#include <ostream>
#include <fstream>

class Client{
private:
    LoggingProvider logger = LoggingProvider::getInstance();
    nlohmann::json settings;
    Strategy strategy;

    void connectToServer(short unsigned int port);
    void play(Player& client);
    int getChoice(int oponentsLastChoice);

    // Network
    void sendRequest(Player& client, Request& request);
    Response getResponse(Player& client);
public:
    Client(nlohmann::json& config);
    ~Client();
};