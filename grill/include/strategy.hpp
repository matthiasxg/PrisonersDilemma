#pragma once
#include "logging_provider.hpp"

class Strategy {
private:
    int currentRound{1};
    int oponentsLastChoice{-1};
    LoggingProvider logger = LoggingProvider::getInstance();
public:
    Strategy() {};
    void nextRound();
    void setOponentsLastChoice(int choice);
    int getNextChoice(int oponentsLastChoice);
};