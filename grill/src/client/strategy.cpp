/*
 *  Name: Matthias Grill
 *  Class: 5BHIF
 *  Date: 27.02.2019
 *  File: src/client/strategy.cpp
 */
#include "strategy.hpp"

using namespace std;

void Strategy::setOponentsLastChoice(int choice)
{
    this->oponentsLastChoice = choice;
}

void Strategy::nextRound()
{
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
 * Already implemented strategies:
 *  - int alwaysCooperate(); 
 *  - int alwaysDefects(); 
 *  - int randomPlayer();
 *  - int tiltForThat(int oponentsLastChoice); 
 *  - int grim(int oponentsLastChoice);
 * 
 */
int Strategy::getNextChoice(int oponentsLastChoice) {
    if (oponentsLastChoice == -1) {
        return 1;
    }
    return oponentsLastChoice;
}

// always cooperates
int Strategy::alwaysCooperate() {
    return 1;
}

// always defects
int Strategy::alwaysDefects() {
    return 0;
}

// Makes a random move
int Strategy::randomPlayer() {
    random_device rd;
    mt19937 gen{rd()};
    uniform_int_distribution<> dis{0, 1};
    return dis(gen);
}

// cooperates on the first move then plays what its opponent played the previous move
int Strategy::tiltForThat(int oponentsLastChoice) {
    if (oponentsLastChoice == -1) {
        return 1;
    }
    return oponentsLastChoice;
}

// cooperates until the opponent defects and thereafter always defects
int Strategy::grim(int oponentsLastChoice) {
    if (oponentsLastChoice == 1) {
        this->grimVar = false;
    }
    if (this->grimVar) {
        return 1;
    }
    return 0;
}