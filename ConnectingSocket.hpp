#ifndef CONNECTINGSOCKET_HPP
#define CONNECTINGSOCKET_HPP

#include "SimpleSocket.hpp"

namespace FRP
{
    class ConnectingSocket : public SimpleSocket
    {
        // Constructor
        ConnectingSocket(int domain, int service, int protocol, int port, u_long interface);
        // Virtual function from parent
        int connect_to_network(int sock, struct sockaddr_in address);
    };
}

#endif