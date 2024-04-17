#ifndef BINDINGSOCKET_HPP
#define BINDINGSOCKET_HPP

#include "SimpleSocket.hpp"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace FRP
{
    class BindingSocket : public SimpleSocket
    {
        private:
            // The binding var stores the vaue 0 if successful
            int binding;
            // Virtual fiunction from parent to establish binding
            void connect_to_network(int sock, struct sockaddr_in address);

        public:
            // CONSTRUCTOR
            // Parameterized constructor is required for this class
            BindingSocket(int domain, int service, int protocol, int port, u_long interface);
            // GETTERS
            int get_binding();

            ~BindingSocket();
    };
}

#endif