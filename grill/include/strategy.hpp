/*
 *  Name: Matthias Grill
 *  Class: 5BHIF
 *  Date: 27.02.2019
 *  File: include/strategy.hpp
 */
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