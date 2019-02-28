#include "server.hpp"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

json getJsonSettings() {
    json result;
    std::ifstream i("../src/static/server_config.json");
    i >> result;
    return result;
}

//int main(int argc, char* argv[]) {
int main() {
    json server_config = getJsonSettings();
    Server server(ref(server_config));
    return 0;
}
