#pragma once
#include "logging_provider.hpp"
#include "PrisonersDilemma.pb.h"
#include "PrisonersDilemma.grpc.pb.h"
#include "player.hpp"
#include "json.hpp"

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
#include <iostream>
#include <fstream>

class Server {
private:
    LoggingProvider logger = LoggingProvider::getInstance();
    std::vector<Player> clients;
    nlohmann::json settings;

    void startServer(short unsigned int port);
    void handleClient(Player& client);
    bool gamePreparation(Player& client);
    void play(Player& client);
    void calculateResult(int firstPlayerChoice, int secondPlayerChoice, bool lastRound);
    void punish(int firstTime, int secondTime, int firstChoice, int secondChoice, bool lastRound);

    // Network
    void sendResponse(Player& client, Response& response);
    Request getRequest(Player& client);
public:
    Server(nlohmann::json& config);
    ~Server();
};