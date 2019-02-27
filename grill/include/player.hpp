#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"
#include "asio.hpp"
#pragma GCC diagnostic pop

#include <string>
#include <vector>
#include <thread>

class Player {
private:
    std::shared_ptr<asio::ip::tcp::socket> socket;
    std::string name{""};
    int id{-1};
    int choice{-1};
    int detentionTime{0};
public:
    Player(std::shared_ptr<asio::ip::tcp::socket> socket);
    void addDetentionTime(int time);

    // Getter
    int getId() { return this->id; };
    std::string getName() { return this->name; };
    std::shared_ptr<asio::ip::tcp::socket> getSocket() { return this->socket; };
    int getChoice() { return this->choice; };
    int getDetentionTime() { return this->detentionTime; };

    // Setter
    void setName(std::string name) { this->name = name; };
    void setId(int id) { this->id = id; };
    void setChoice(int choice) { this->choice = choice; };
};