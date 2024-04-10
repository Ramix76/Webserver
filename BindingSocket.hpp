#ifndef BINDINGSOCKET_HPP
#define BINDINGSOCKET_HPP

#include "SimpleSocket.hpp"

namespace FRP
{
    class BindingSocket: public SimpleSocket
    {
        public:
            // Constructor
            BindingSocket(int domain, int service, int protocol, int port, u_long interface);
            // Virtual fiunction from parent
            int connect_to_network(int sock, struct sockaddr_in address);
    };
}

#endif