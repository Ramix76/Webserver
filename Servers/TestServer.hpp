#ifndef TESTSERVER_HPP
#define TESTSERVER_HPP

#include "SimpleServer.hpp"

namespace FRP
{
    class TestServer : public SimpleServer
    {
        private:
            char buffer[30000] = {0};
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