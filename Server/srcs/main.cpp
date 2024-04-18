#include "SocketServer.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }

    std::string configFile = argv[1];
    SocketServer server(configFile);
    server.Start();

    return 0;
}
