/*
 *  Name: Matthias Grill
 *  Class: 5BHIF
 *  Date: 27.02.2019
 *  File: src/server/server.cpp
 */
#include "server.hpp"

using namespace asio::ip;
using namespace std;
using json = nlohmann::json;

std::mutex m;
std::condition_variable cv;
bool ready = false;

//Networking stuff
void Server::sendResponse(Player& client, Response& response) {
    try {
        string s{};
        response.SerializeToString(&s);
        asio::write(*(client.getSocket()), asio::buffer(s + "ENDOFMESSAGE"));
        logger.debug("Sent response of type " + to_string(response.type()) + 
                    " to " + to_string(client.getId()));
    } catch(const std::exception& e) {
        logger.error("Server send response error: ");
        std::cerr << e.what() << '\n';
    }
}

Request Server::getRequest(Player& client) {
    try {
        asio::streambuf buffer;
        asio::read_until(*client.getSocket(), buffer, "ENDOFMESSAGE");

        asio::streambuf::const_buffers_type bufs = buffer.data();
        string requestString{asio::buffers_begin(bufs), asio::buffers_begin(bufs) + buffer.size()}; 
        requestString.erase(requestString.size() - 12);

        Request request;
        request.ParseFromString(requestString);

        logger.debug("Got request of type " + to_string(request.type()) + 
                    " from " + to_string(client.getId()));

        return request;
    } catch(const std::exception& e) {
        logger.error("Server get request error: ");
        std::cerr << e.what() << '\n';
    }
    return Request{};    
}

bool Server::gamePreparation(Player& client) {
    Request request = getRequest(ref(client));
    if (request.type() == Request::START) {
        client.setName(request.name());
        client.setId(clients.size());

        logger.info("Prisoner " + client.getName() + " connected");
        logger.debug("Prisoner " + client.getName() + 
                     " has id " + to_string(client.getId()));
        
        // Confirmation
        Response response;
        response.set_type(Response::CONFIRM);
        response.set_name(client.getName());
        response.set_id(client.getId());
        sendResponse(ref(client), ref(response));

        // Wait for second game partner
        logger.debug(to_string(client.getId()) + " waits for second prisoner");
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, []{return ready;});
        logger.debug("Two players are connected");

        // Notify client about game start
        response.set_type(Response::GAMESTART);
        sendResponse(ref(client), ref(response));
        
        return true;
    }
    return false;
}

void Server::handleClient(Player& client) {
    if(gamePreparation(ref(client))) {
        logger.info("Preperations completed");
        play(ref(client));
        
        // Disconnect
        client.setId(-1);
        while (clients.at(0).getId() != -1 || clients.at(1).getId() != -1) {}
        clients.clear();
        clients.reserve(2);
        ready = false;

        logger.info("Server restarts...");
    } else {
        logger.error("Game preparation failed\n");
        exit(1);
    }
}

void Server::play(Player& client) {
    bool lastRound{false};
    logger.info("Game started");

    for (int i{0}; i < settings["rounds"]; i++) {
        Request request = getRequest(ref(client));

        if (request.type() == Request::PLAY) {
            client.setChoice(request.choice());

            if (client.getId() == 1) {  // Player 1 handles everything

                while(clients.at(1).getChoice() == -1){} // Wait for second player
                if (i + 1 == settings["rounds"]) { lastRound = true; }
                calculateResult(client.getChoice(), clients.at(1).getChoice(), lastRound);
                clients.at(1).setChoice(-1);

            } else if (client.getId() == 2) { // Player 2 waits for Player 1
                while(clients.at(1).getChoice() == -1){}
            }
        } else {
            logger.error("Sever got no play request\n");
            exit(1);
        }
    }
}

void Server::calculateResult(int first, int second, bool lastRound) {
    if (first == 0 && second == 0) {
        punish(2, 2, first, second, lastRound);
    } else if (first == 0 && second == 1) {
        punish(6, 1, first, second, lastRound);
    } else if (first == 1 && second == 0) {
        punish(1, 6, first, second, lastRound);
    } else if (first == 1 && second == 1) {
        punish(4, 4, first, second, lastRound);
    } else {
        logger.error("Incorrect parameters in function calculateResult\n");
        logger.error("Got first: " + to_string(first) + " second: " + to_string(second));
        exit(1);
    }
}

void Server::punish(int punishFirst, int punishSecond, int choiceFirst, int choiceSecond, bool lastRound){
    clients.at(0).addDetentionTime(punishFirst);
    clients.at(1).addDetentionTime(punishSecond);

    // Send them the info
    Response response;

    // Last round
    if (lastRound) { response.set_lastround(true); }

    // First Player
    response.set_type(Response::PLAY);
    response.set_diff(punishFirst);
    response.set_oponentsdiff(punishSecond);
    response.set_points(clients.at(0).getDetentionTime());
    response.set_oponentspoints(clients.at(1).getDetentionTime());
    response.set_oponentschoice(choiceSecond);
    sendResponse(ref(clients.at(0)), ref(response));

    // Second Player
    response.set_type(Response::PLAY);
    response.set_diff(punishSecond);
    response.set_oponentsdiff(punishFirst);
    response.set_points(clients.at(1).getDetentionTime());
    response.set_oponentspoints(clients.at(0).getDetentionTime());
    response.set_oponentschoice(choiceFirst);
    sendResponse(ref(clients.at(1)), ref(response));
}

void Server::startServer(short unsigned int port) {
    try {
        asio::io_context ctx;
        tcp::endpoint ep{tcp::v4(), port};
        tcp::acceptor acceptor{ctx, ep};

        logger.info("Server started");
        logger.debug("Port " + to_string(port));

        acceptor.listen();
        
        // Reserve for two clients
        clients.reserve(2);

        while(true){
            
            // 2 = Number of players
            while (clients.size() < 2) {
                Player player = make_shared<asio::ip::tcp::socket>(acceptor.accept());
                this->clients.push_back(player);

                thread t(&Server::handleClient, this, ref(clients.at(clients.size() - 1)));
                t.detach();
            }

            // Notify about game start
            ready = true;
            cv.notify_all();
        }
    } catch(const std::exception& e) {
        logger.error("Start server error: ");
        std::cerr << e.what() << '\n';
    }
}

Server::Server(json& config) {

    // Set settings and logger level
    this->settings = config;
    if (settings["debug"]) {
        spdlog::set_level(spdlog::level::debug);
    }

    startServer(settings["port"]);
}