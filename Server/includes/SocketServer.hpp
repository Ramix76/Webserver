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

class SocketServer {
private:
    int listening;
    sockaddr_in hint;
    sockaddr_in client;
    socklen_t clientSize;
    int clientSocket;

public:
    SocketServer(int port);
    ~SocketServer();
    void Start();
};

#endif /* SOCKETSERVER_HPP */