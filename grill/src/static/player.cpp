#include "player.hpp"

using namespace std;

Player::Player(std::shared_ptr<asio::ip::tcp::socket> socket){
    this->socket = socket;
}