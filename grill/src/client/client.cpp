#include "client.hpp"

using namespace asio::ip;
using namespace std;
using json = nlohmann::json;

void Client::sendRequest(Player& client, Request& request) {
    string s;
    request.SerializeToString(&s);

    asio::write(*(client.getSocket()), asio::buffer(s + "ENDOFMESSAGE"));
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

    auto result = resolver.resolve(settings["serverIp"], to_string(port));

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

int Client::getChoice(int oponentsLastChoice) {
    logger.info("Please make a decision!");
    int result{-1};
    if (settings["playOnCommandLine"]) {
        logger.info("0 = not guilty, 1 = guilty");
        logger.input("");
        cin >> result;
    } else {
        result = strategy.getNextChoice(oponentsLastChoice);
    }

    if (result == 0) {
        logger.info("You said you are not guilty");
    } else if (result == 1) {
        logger.info("You said you are guilty");
    }
    return result;
}

void Client::play(Player& client) {
    int oponentsLastChoice{-1};
    while (true) {
        Request request;
        request.set_type(Request::PLAY);
        request.set_choice(getChoice(oponentsLastChoice));
        sendRequest(ref(client), ref(request));

        Response response = getResponse(ref(client));
        if (response.type() == Response::PLAY) {
            if (response.oponentschoice() == 0) {
                logger.otherPlayer("The other prisoner said that he is NOT guilty");
            } else {
                logger.otherPlayer("The other prisoner said that he is GUILTY");
            }
            logger.info("So you got punished for " + to_string(response.diff()) + " years");
            logger.otherPlayer("The other prisoner got punished for " + to_string(response.oponentsdiff()) + " years");
            logger.info("Now you have a total detention time of " + to_string(response.points()) + " years");
            logger.otherPlayer("The other prisoner has a total detention time of " + to_string(response.oponentspoints()) + " years");
            
            // Strategy
            oponentsLastChoice = response.oponentschoice();
            strategy.nextRound();
        }

        if (response.lastround()) { 
            logger.info("--------------------------------------------");
            logger.info("Game is over, thanks for playing");
            logger.info("Stats: ");
            logger.info("Your detention time: " + to_string(response.points()) + " years");
            logger.info("The other prisoner has a detention time of: " + to_string(response.oponentspoints()) + " years");
            if (response.points() > response.oponentspoints()) {
                logger.info("You are the LOOSER :-(");
            } else if (response.points() < response.oponentspoints()) {
                logger.info("You are the WINNER :-)");
            } else {
                logger.info("DRAW, have fun in the prison ;-)");
            }
            logger.info("--------------------------------------------");
            break; 
        }
    }
    
    // Disconnect
    client.getSocket()->close();
}

Client::Client(json& config) {
    this->settings = config;
    if (settings["debug"]) {
        spdlog::set_level(spdlog::level::debug);
    }
    connectToServer(settings["port"]);
}

Client::~Client() {}