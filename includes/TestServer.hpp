#ifndef TESTSERVER_HPP
#define TESTSERVER_HPP

#include "SimpleServer.hpp"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace FRP
{
    class TestServer : public SimpleServer
    {
        private:
            char buffer[30000];
            int new_socket;
            void accepter();
            void handler();
            void responder();
        public:
            TestServer();
            void launch();
    };
}

#endif