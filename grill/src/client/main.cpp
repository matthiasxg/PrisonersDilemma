/*
 *  Name: Matthias Grill
 *  Class: 5BHIF
 *  Date: 27.02.2019
 *  File: src/client/main.cpp
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include "clipp.h"
#pragma GCC diagnostic pop

#include "client.hpp"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;
using namespace clipp;

json getJsonSettings() {
    json result;
    std::ifstream i("../src/static/client_config.json");
    i >> result;
    return result;
}

int main(int argc, char* argv[]) {

    // Commandline interface
    short unsigned int port{0};
    string ip{""};
    bool help{false};
    bool debug{false};
    bool strategy{false};

    auto cli = (
        option("-st", "--strategy").set(strategy).doc("Your own strategy will be used"),
        option("-s", "--server").doc("Ip adress of server") & value("ip", ip),
        option("-p", "--port").doc("Port to connect to") & value("port", port),
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

    // Set values to JSON settings
    json client_config = getJsonSettings();
    if(ip != "") client_config["serverIp"] = ip;
    if(port != 0) client_config["port"] = port;
    if(strategy) client_config["playOnCommandLine"] = false;
    if(debug) client_config["debug"] = true;

    // Start client with JSON settings
    Client client(ref(client_config));
    return 0;
}
