#ifndef CONNECTINGSOCKET_HPP
#define CONNECTINGSOCKET_HPP

#include "SimpleSocket.hpp"

namespace FRP
{
    class ConnectingSocket : public SimpleSocket
    {
        private:
            int connection;
        
        public:
            // CONSTRUCTOR
            ConnectingSocket(int domain, int service, int protocol, int port, u_long interface);
            int get_connection() const { return (connection);}

            ~ConnectingSocket();
        
        protected:
            // Virtual function from parent
            void connect_to_network(int sock, struct sockaddr_in address);
    };
}

#endif