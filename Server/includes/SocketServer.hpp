#ifndef SOCKETSERVER_HPP
#define SOCKETSERVER_HPP

#include <string>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/event.h>
#include <vector>
#include <sstream>
#include "Logger.hpp"

#define MAX_EVENTS 10

struct ServerConfig
{
    int port;
    std::string serverName;
    std::string errorPage404;
    // Agrega más propiedades según sea necesario
};

class SocketServer
{
public:
    SocketServer(const std::string& configFile, Logger &logger);
    ~SocketServer(); // Destructor

    void Start();

private:
    int listening;
    int kq;
    ServerConfig config;
    Logger &logger;

    void acceptConnection();
    void handleRequest(int clientSocket);
    void sendResponse(int clientSocket, const std::string& response);
};

#endif /* SOCKETSERVER_HPP */