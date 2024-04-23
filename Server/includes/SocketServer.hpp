#ifndef SOCKETSERVER_HPP
#define SOCKETSERVER_HPP

#include "Logger.hpp"
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
#include <thread>

// Define una estructura para almacenar la configuración del servidor
struct ServerConfig
{
    int port;
    std::string serverName;
    std::string errorPage404;
    // Agrega más propiedades según sea necesario
};

class SocketServer
{
    private:
        int listening;
        sockaddr_in hint;
        sockaddr_in client;
        socklen_t clientSize;
        int clientSocket;
        ServerConfig config; // Propiedad para almacenar la configuración del servidor
        Logger logger; // Agrega un miembro de datos Logger
        std::ofstream logFile; // Archivo de registro

    public:
        SocketServer(const std::string& configFile);
        ~SocketServer();
        void Start();

        // void handleRequest(const std::string& request);
        std::string getHTTPResponse(const std::string& method, const std::string& path);
        void handleRequest(int clientSocket);
        void handleHTTPRequest(const std::string& request, int clientSocket);
        void handleGETRequest(const std::string& path, int clientSocket);
        void handlePOSTRequest(const std::string& path, int clientSocket, const std::string& request);
        void handleDELETERequest(const std::string& path, int clientSocket);
        void sendHTTPResponse(int clientSocket, const std::string& status, const std::string& content, int statusCode);
        void sendResponse(int clientSocket, const std::string& response);
};

#endif /* SOCKETSERVER_HPP */