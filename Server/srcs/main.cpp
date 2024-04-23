#include "SocketServer.hpp"
#include "Logger.hpp"


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }

    std::string configFile = argv[1];

    // Crear una instancia de Logger
    Logger logger("logfile.txt");

    // Registrar un mensaje indicando que se está iniciando el servidor
    logger.log("IStarting the server...");

    // Crear una instancia de SocketServer y comenzar el servidor
    SocketServer server(configFile);

    // Registrar un mensaje indicando que el servidor ha comenzado
    logger.log("The server has started.");

    server.Start();

    return 0;
}
