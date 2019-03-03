/*
 *  Name: Matthias Grill
 *  Class: 5BHIF
 *  Date: 27.02.2019
 *  File: src/client/strategy.cpp
 */
#include "strategy.hpp"

using namespace std;

void Strategy::setOponentsLastChoice(int choice) {
    this->oponentsLastChoice = choice;
}

void Strategy::nextRound() {
    this->currentRound++;
}

/*
 *
 * IMPLEMENT YOUR STRATEGY HERE
 * 0 = not guilty, 1 = guilty
 * In the first round oponentsLastChoice will be -1
 * 
 * Available variables:
 *  - int oponentsLastChoice (Oponents choice from last round)
 *  - int currentRound (Current round number, start value: 1)
 * 
 */
int Strategy::getNextChoice(int oponentsLastChoice) {
    if (oponentsLastChoice == -1) {
        return 1;
    } else {
        return 0;
    }
    return 0;
}