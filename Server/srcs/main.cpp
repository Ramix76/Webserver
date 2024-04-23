#include "SocketServer.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }

    std::string configFile = argv[1];

    // Create an instance of Logger
    Logger logger("logfile.txt");

    // Log a message indicating that the server is starting
    // logger.log("Starting the server...");

    // Create an instance of SocketServer and start the server
    SocketServer server(configFile, logger);

    // Log a message indicating that the server has started
    logger.log("The server has started.");

    server.Start();

    return 0;
}
