/*
 *  Name: Matthias Grill
 *  Class: 5BHIF
 *  Date: 27.02.2019
 *  File: include/strategy.hpp
 */
#pragma once
#include "logging_provider.hpp"
#include <stdlib.h>

class Strategy {
private:
    int currentRound{1};
    int oponentsLastChoice{-1};
    LoggingProvider logger = LoggingProvider::getInstance();
    bool grimVar{true};
public:
    Strategy() {};
    void nextRound();
    void setOponentsLastChoice(int choice);
    int getNextChoice(int oponentsLastChoice);

    /*
     *  Implemented Strategies
     */

    // always cooperates
    int alwaysCooperate(); 

    // always defects
    int alwaysDefects(); 

    // Makes a random move
    int randomPlayer();

    // cooperates on the first move then plays what its opponent played the previous move
    int tiltForThat(int oponentsLastChoice); 

    // cooperates until the opponent defects and thereafter always defects
    int grim(int oponentsLastChoice);
};