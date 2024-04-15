#ifndef LISTENINGSOCKET_HPP
#define LISTENINGSOCKET_HPP

#include "BindingSocket.hpp"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace FRP
{
    class ListeningSocket : public BindingSocket
    {
        private:
            int backlog;
            int listening;

        public:
            ListeningSocket(int domain, int service, int protocol, int port, u_long interface, int bcklg);
            void start_listening();
            int get_backlog();
            int get_listening();
    };
}

#endif