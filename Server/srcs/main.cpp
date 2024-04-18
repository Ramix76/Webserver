#include "SocketServer.hpp"

int main() {
    int port = 54000;
    SocketServer server(port);
    server.Start();
    return 0;
}
