#include "SocketServer.hpp"

class HandleRequestWrapper {
public:
    HandleRequestWrapper(SocketServer* server, int clientSocket)
        : server_(server), clientSocket_(clientSocket) {}

    void operator()() const {
        server_->handleRequest(clientSocket_);
    }

private:
    SocketServer* server_;
    int clientSocket_;
};