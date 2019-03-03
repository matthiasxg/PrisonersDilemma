/*
 *  Name: Matthias Grill
 *  Class: 5BHIF
 *  Date: 27.02.2019
 *  File: static/player.cpp
 */
#include "player.hpp"

using namespace std;

Player::Player(std::shared_ptr<asio::ip::tcp::socket> socket){
    this->socket = socket;
}

void Player::addDetentionTime(int time) {
    this->detentionTime += time;
}