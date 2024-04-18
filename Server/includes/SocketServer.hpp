#ifndef SOCKETSERVER_HPP
#define SOCKETSERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring> // For memset
#include <cerrno> // For errno
#include <fstream>
#include <sstream>

// Define una estructura para almacenar la configuración del servidor
struct ServerConfig {
    int port;
    std::string serverName;
    std::string errorPage404;
    // Agrega más propiedades según sea necesario
};

class SocketServer {
private:
    int listening;
    sockaddr_in hint;
    sockaddr_in client;
    socklen_t clientSize;
    int clientSocket;
    ServerConfig config; // Propiedad para almacenar la configuración del servidor
    std::ofstream logFile; // Archivo de registro

public:
    SocketServer(const std::string& configFile);
    ~SocketServer();
    void Start();

    void handleRequest(const std::string& request);
};

#endif /* SOCKETSERVER_HPP */