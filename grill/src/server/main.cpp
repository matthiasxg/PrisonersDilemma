/*
 *  Name: Matthias Grill
 *  Class: 5BHIF
 *  Date: 27.02.2019
 *  File: src/server/main.cpp
 */
#include "server.hpp"
#include "json.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include "clipp.h"
#pragma GCC diagnostic pop


using namespace std;
using json = nlohmann::json;
using namespace clipp;

json getJsonSettings() {
    json result;
    std::ifstream i("../src/static/server_config.json");
    i >> result;
    return result;
}

int main(int argc, char* argv[]) {
    
    // Commandline interface
    short unsigned int port{0};
    bool help{false};
    bool debug{false};
    int rounds{-1};

    auto cli = (
        option("-p", "--port").doc("Port to connect to") & value("port", port),
        option("-r", "--rounds").doc("Number of rounds you want to play") & value("rounds", rounds),
        option("-d", "--debug").set(debug).doc("Debug mode"),
        option("-h", "--help").set(help).doc("Print this help message and exit")
    );

    if (!parse(argc, argv, cli)) {
        cout << make_man_page(cli, argv[0]);
        return 1;
    } else if (help) {
        cout << make_man_page(cli, argv[0]);
        return 1;
    }

    // Set values to settings
    json server_config = getJsonSettings();
    if(port != 0) server_config["port"] = port;
    if(rounds != -1) server_config["rounds"] = rounds;
    if(debug) server_config["debug"] = true;

    // Start server
    Server server(ref(server_config));
    return 0;
}
