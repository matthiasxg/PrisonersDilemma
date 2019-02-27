#include "server.hpp"

using namespace asio::ip;
using namespace std;
using json = nlohmann::json;

std::mutex m;
std::condition_variable cv;
bool ready = false;

//Networking stuff
void Server::sendResponse(Player& client, Response& response) {
    string s{};
    response.SerializeToString(&s);
    asio::write(*(client.getSocket()), asio::buffer(s + "ENDOFMESSAGE"));
}

Request Server::getRequest(Player& client) {
    asio::streambuf buffer;
    asio::read_until(*client.getSocket(), buffer, "ENDOFMESSAGE");

    asio::streambuf::const_buffers_type bufs = buffer.data();
    string requestString{asio::buffers_begin(bufs), asio::buffers_begin(bufs) + buffer.size()}; 
    requestString.erase(requestString.size() - 12);

    Request request;
    request.ParseFromString(requestString);

    return request;
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
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, []{return ready;});

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
    }
}

void Server::play(Player& client) {
    for (int i{0}; i < settings["rounds"]; i++) {
        Request request = getRequest(ref(client));
        if (request.type() == Request::PLAY) {
            client.setChoice(request.choice());
            int firstPlayerChoice{-1};
            int secondPlayerChoice{-1};
            switch (client.getId())
            {
                case 1:
                    firstPlayerChoice = client.getChoice();
                    while(clients.at(1).getChoice() == -1) {}
                    secondPlayerChoice = clients.at(1).getChoice();
                    break;
                case 2:
                    secondPlayerChoice = client.getChoice();
                    while(clients.at(0).getChoice() == -1) {}
                    firstPlayerChoice = clients.at(0).getChoice();

                    // Just calculate once
                    calculateResult(firstPlayerChoice, secondPlayerChoice);

                    break;
                default:
                    logger.warning("-1");
                    break;
            }
        }
    }
}

void Server::calculateResult(int first, int second) {
    if (first == 0 && second == 0) {
        punish(2, 2);
    } else if (first == 0 && second == 1) {
        punish(6, 1);
    } else if (first == 1 && second == 0) {
        punish(1, 6);
    } else if (first == 1 && second == 1) {
        punish(4, 4);
    } else {
        logger.warning("Error");
    }
}

void Server::punish(int first, int second){
    clients.at(0).addDetentionTime(first);
    clients.at(1).addDetentionTime(second);
    
    // Send them the info
    Response response;
    response.set_type(Response::PLAY);
    response.set_diff(first);
    response.set_points(clients.at(0).getDetentionTime());
    sendResponse(ref(clients.at(0)), ref(response));

    response.set_type(Response::PLAY);
    response.set_diff(second);
    response.set_points(clients.at(1).getDetentionTime());
    sendResponse(ref(clients.at(1)), ref(response));
}

void Server::startServer() {
    asio::io_context ctx;
    tcp::endpoint ep{tcp::v4(), port};
    tcp::acceptor acceptor{ctx, ep};

    logger.info("Server started");
    logger.debug("Listening on port " + to_string(port));

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

        ready = true;
        cv.notify_all();
    }
}

void Server::getJsonSettings() {
    std::ifstream i("../src/static/config.json");
    i >> settings;
}

Server::Server(short unsigned int port) {
    this->port = port;
    getJsonSettings();
    if (settings["debug"]) {
        spdlog::set_level(spdlog::level::debug);
    }
    startServer();
}


Server::~Server() {
    
}