#include "client.hpp"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

json getJsonSettings() {
    json result;
    std::ifstream i("../src/static/client_config.json");
    i >> result;
    return result;
}

//int main(int argc, char* argv[]) {
int main() {
    json settings = getJsonSettings();
    Client client(ref(settings));
    return 0;
}
