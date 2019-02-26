#include "client.hpp"

using namespace asio::ip;
using namespace std;

void Client::sendRequest(Player& client, Request& request) {
    string s;
    request.SerializeToString(&s);

    asio::write(*client.getSocket(), asio::buffer(s + "ENDOFMESSAGE"));
}

Response Client::getResponse(Player& client) {
    asio::streambuf buffer;
    asio::read_until(*client.getSocket(), buffer, "ENDOFMESSAGE");

    asio::streambuf::const_buffers_type bufs = buffer.data();
    string responseString{asio::buffers_begin(bufs), asio::buffers_begin(bufs) + buffer.size()}; 
    responseString.erase(responseString.size() - 12);

    Response response;
    response.ParseFromString(responseString);

    return response;
}

void Client::connectToServer(short unsigned int port) {
    asio::io_context ctx;
    tcp::resolver resolver{ctx};

    auto result = resolver.resolve("localhost", to_string(port));

    tcp::socket socket(ctx);

    asio::connect(socket, result);

    auto sck = make_shared<asio::ip::tcp::socket>(move(socket));
    Player myPlayer{sck};

    // I want to play
    Request request;
    request.set_type(Request::START);
    request.set_name("Matthias");

    sendRequest(ref(myPlayer), ref(request));

    // Get confirmation
    Response response = getResponse(ref(myPlayer));
    if (response.type() == Response::CONFIRM) {
        myPlayer.setName(response.name());
        myPlayer.setId(response.id());

        logger.info("I am playing as " + myPlayer.getName());
        logger.debug("My ID is " + to_string(myPlayer.getId()));
    }

    // Wait for game start
    response = getResponse(ref(myPlayer));
    if (response.type() == Response::GAMESTART) {
        logger.info("Two prisoners are connected, game stars");
    }
}

Client::Client(short unsigned int port) {
    spdlog::set_level(spdlog::level::debug);
    this->port = port;
    connectToServer(port);
}

Client::~Client() {}

