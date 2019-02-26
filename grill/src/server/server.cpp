#include "server.hpp"

using namespace asio::ip;
using namespace std;

std::mutex m;
std::condition_variable cv;
bool ready = false;

//Networking stuff
void Server::sendResponse(Player& client, Response& response) {
    string s{};
    response.SerializeToString(&s);
    asio::write(*client.getSocket(), asio::buffer(s + "ENDOFMESSAGE"));
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

void Server::handleClient(Player client) {
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
    }
}

void Server::startServer() {
    asio::io_context ctx;
    tcp::endpoint ep{tcp::v4(), port};
    tcp::acceptor acceptor{ctx, ep};

    logger.info("Server started");
    logger.debug("Listening on port " + to_string(port));
    acceptor.listen();

    while(true){

        while (clients.size() < 2) {
            auto socket = make_shared<asio::ip::tcp::socket>(acceptor.accept());

            this->clients.push_back(Player(socket));

            thread t([this] {
                handleClient(move(this->clients.at(clients.size() - 1)));
            });
            t.detach();
        }
        ready = true;
        cv.notify_all();
    }
}

Server::Server(short unsigned int port) {
    spdlog::set_level(spdlog::level::debug);
    this->port = port;  
    startServer();
}


Server::~Server() {
    
}