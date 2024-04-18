#include "SocketClient.hpp"

SocketClient::SocketClient() {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
}

SocketClient::~SocketClient() {
    close(sock);
}

bool SocketClient::Connect(const std::string& ipAddress, int port) {
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

    int connectRes = connect(sock, (sockaddr *)&hint, sizeof(hint));
    if (connectRes == -1) {
        std::cerr << "Error connecting to server: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool SocketClient::Send(const std::string& msg) {
    int sendRes = send(sock, msg.c_str(), msg.size() + 1, 0);
    if (sendRes == -1) {
        std::cerr << "Error sending data: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

std::string SocketClient::Receive() {
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    int bytesRcv = recv(sock, buffer, sizeof(buffer), 0);
    if (bytesRcv == -1) {
        std::cerr << "Error receiving data: " << strerror(errno) << std::endl;
        return "";
    }
    return std::string(buffer, bytesRcv);
}