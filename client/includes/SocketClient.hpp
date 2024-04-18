#ifndef SOCKETCLIENT_HPP
#define SOCKETCLIENT_HPP

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string>
#include <cstring>
#include <cerrno>

class SocketClient {
private:
    int sock;
    sockaddr_in hint;

public:
    SocketClient();
    ~SocketClient();

    bool Connect(const std::string& ipAddress, int port);
    bool Send(const std::string& msg);
    std::string Receive();
};

#endif // SOCKETCLIENT_HPP