/*
 *  Name: Matthias Grill
 *  Class: 5BHIF
 *  Date: 27.02.2019
 *  File: src/client/client.cpp
 */
#include "client.hpp"

using namespace asio::ip;
using namespace std;
using json = nlohmann::json;

void Client::sendRequest(Player& client, Request& request) {
    try {
        string s;
        request.SerializeToString(&s);
        asio::write(*(client.getSocket()), asio::buffer(s + "ENDOFMESSAGE"));
        logger.debug("Sent request");
    } catch(const std::exception& e) {
        logger.error("Client send request error: ");
        std::cerr << e.what() << '\n';
    }
}

Response Client::getResponse(Player& client) {
    try {
        asio::streambuf buffer;
        logger.debug("Wait for response");
        asio::read_until(*client.getSocket(), buffer, "ENDOFMESSAGE");
        logger.debug("Got response");

        asio::streambuf::const_buffers_type bufs = buffer.data();
        string responseString{asio::buffers_begin(bufs), 
                            asio::buffers_begin(bufs) + buffer.size()}; 
        responseString.erase(responseString.size() - 12);

        Response response;
        response.ParseFromString(responseString);

        return response;
    } catch(const std::exception& e) {
        logger.error("GetResponse error: ");
        std::cerr << e.what() << '\n';
    }
    return Response{};
}

void Client::connectToServer(short unsigned int port) {
    logger.info("Client started");
    logger.debug("Server IP: " + settings["serverIp"].get<std::string>());
    logger.debug("Port to connect to: " + to_string(port));

    asio::io_context ctx;
    tcp::resolver resolver{ctx};
    auto result = resolver.resolve(settings["serverIp"], to_string(port));
    tcp::socket socket(ctx);
    
    try {
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
        } else {
            logger.error("Got no play confirmation\n");
            exit(1);
        }

        // Wait for game start
        response = getResponse(ref(myPlayer));
        if (response.type() == Response::GAMESTART) {
            logger.info("Game starts");
        } else {
            logger.error("Got no game start confirmation\n");
            exit(1);
        }

        // Game starts
        play(ref(myPlayer));
    }
    catch(asio::system_error& e) {
        logger.error("Error: ");
        std::cerr << e.what() << '\n';
    }
}

int Client::getChoice(int oponentsLastChoice) {
    string input{"-1"};
    int result{-1};
    if (settings["playOnCommandLine"]) {
        logger.info("Please make a decision!");
        logger.info("0 or n = not guilty");
        logger.info("1 or y = guilty");
        logger.input("");
        cin >> input;
        try {
            if (input == "y" || input == "Y") {
                result = 1;
            } else if (input == "n" || input == "N") {
                result = 0;
            } else {
                result = stoi(input);
            }
        }
        catch(...) {
            logger.error("Invalid input: " + input + "\n");
            return -1;
        }
    } else {
        result = strategy.getNextChoice(oponentsLastChoice);
    }

    if (result == 0) {
        logger.info("You said you are NOT guilty");
    } else if (result == 1) {
        logger.info("You said you are GUILTY");
    } else {
        logger.error("Invalid input: " + to_string(result) + "\n");
        return -1;
    }
    return result;
}

void Client::play(Player& client) {
    int oponentsLastChoice{-1};

    while (true) {

        // Create Request and send choice
        Request request;
        request.set_type(Request::PLAY);
        int result = getChoice(oponentsLastChoice);
        while (result == -1) { result = getChoice(oponentsLastChoice); }
        request.set_choice(result);
        sendRequest(ref(client), ref(request));

        // Wait for response with result
        Response response = getResponse(ref(client));
        if (response.type() == Response::PLAY) {
            if (response.oponentschoice() == 0) {
                logger.otherPlayer("The other prisoner said that he is NOT guilty");
            } else {
                logger.otherPlayer("The other prisoner said that he is GUILTY");
            }

            // Inform client about result
            logger.info("So you got punished for " + to_string(response.diff()) + " years");
            logger.otherPlayer("The other prisoner got punished for " + 
                               to_string(response.oponentsdiff()) + " years");
            logger.info("Now you have a total detention time of " + to_string(response.points()) +
                        " years");
            logger.otherPlayer("The other prisoner has a total detention time of " +
                                to_string(response.oponentspoints()) + " years");
            
            // Strategy
            oponentsLastChoice = response.oponentschoice();
            strategy.nextRound();
        } else {
            logger.error("Got no play response\n");
            exit(1);
        }

        // If last round
        if (response.lastround()) { 
            logger.info("----------------------------------------------------------");
            logger.info("Game is over, thanks for playing");
            logger.info("Stats: ");
            logger.info("Your detention time: " + to_string(response.points()) + " years");
            logger.info("The other prisoner has a detention time of: " + 
                         to_string(response.oponentspoints()) + " years");

            if (response.points() > response.oponentspoints()) {
                logger.info("You are the LOOSER :-(");
            } else if (response.points() < response.oponentspoints()) {
                logger.info("You are the WINNER :-)");
            } else {
                logger.info("DRAW, have fun in the prison ;-)");
            }
            logger.info("----------------------------------------------------------");
            break; 
        }
    }
    
    // Disconnect
    client.getSocket()->close();
}

Client::Client(json& config) {

    // Set log level
    this->settings = config;
    if (settings["debug"]) {
        spdlog::set_level(spdlog::level::debug);
    }

    connectToServer(settings["port"]);
}