#include "client.hpp"

using namespace asio::ip;
using namespace std;
using json = nlohmann::json;

void Client::sendRequest(Player& client, Request& request) {
    string s;
    request.SerializeToString(&s);

    asio::write(*client.getSocket(), asio::buffer(s + "ENDOFMESSAGE"));
}

Response Client::getResponse(Player& client) {
    asio::streambuf buffer;
    asio::read_until(*client.getSocket(), buffer, "ENDOFMESSAGE");

    asio::streambuf::const_buffers_type bufs = buffer.data();
    string responseString{asio::buffers_begin(bufs), 
                          asio::buffers_begin(bufs) + buffer.size()}; 
    responseString.erase(responseString.size() - 12);

    Response response;
    response.ParseFromString(responseString);

    return response;
}

void Client::connectToServer(short unsigned int port) {
    logger.info("Client started");
    logger.debug("Port to connect to: " + to_string(port));

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

    // Select player name
    logger.input("Whats your name? ");
    string name;
    cin >> name;

    request.set_name(name);
    sendRequest(ref(myPlayer), ref(request));

    // Get confirmation
    Response response = getResponse(ref(myPlayer));
    if (response.type() == Response::CONFIRM) {
        myPlayer.setName(response.name());
        myPlayer.setId(response.id());

        logger.info("Welcome " + myPlayer.getName());
        logger.debug("You are player " + to_string(myPlayer.getId()));
    }

    // Wait for game start
    response = getResponse(ref(myPlayer));
    if (response.type() == Response::GAMESTART) {
        logger.info("Game starts");
    }

    // Game starts
    play(ref(myPlayer));
}

void Client::getJsonSettings() {
    std::ifstream i("../src/static/config.json");
    i >> settings;
}

int Client::getChoiceFromCmd() {
    logger.info("Please make a decision!");
    logger.info("0 = not guilty, 1 = guilty");
    logger.input("");
    int result{-1};
    cin >> result;
    if (result == 0) {
        logger.info("You said you are not guilty");
    } else if (result == 1) {
        logger.info("You said you are guilty");
    }
    return result;
}

void Client::play(Player& client) {
    for(int i{0}; i < settings["rounds"]; i++) {
        Request request;
        request.set_type(Request::PLAY);
        request.set_choice(getChoiceFromCmd());
        sendRequest(ref(client), ref(request));

        Response response = getResponse(ref(client));
        if (response.type() == Response::PLAY) {
            logger.info("You got punished: " + to_string(response.diff()));
            logger.info("Overall: " + to_string(response.points()));
        }
    }
    
    // Disconnect
    client.getSocket()->close();
}

Client::Client(short unsigned int port) {
    this->port = port;
    getJsonSettings();
    if (settings["debug"]) {
        spdlog::set_level(spdlog::level::debug);
    }
    connectToServer(port);
}

Client::~Client() {}

