#ifndef SIMPLESOCKET_HPP
#define SIMPLESOCKET_HPP

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>

namespace FRP
{
    class SimpleSocket
    {
        private:
            // The address variable is a struct that contains information about the protocol, type and interface
            // of a given socket. These parameters are specified and set in the constructor.
            struct sockaddr_in address;
            // Once instantiated sock contains the address of a socket object on the network.
            // This should be a non-negative number.
            int sock;    

        public:
            // CONSTRUCTOR
            // A parameterized constructor is required to instantiated the class
            SimpleSocket(int domain, int service, int protocol, int port, u_long interface);
            // VIRTUAL FUNCTIONS
            // This function connects to the network using either bind() or connect() from sys/socket.h
            virtual void connect_to_network(int sock, struct sockaddr_in address) = 0;
            // TESTERS
            // This function ends the program if the connection is not properly established
            // This should be checked frequently
            void test_connection(int);
            // GETTERS 
            struct sockaddr_in get_address();
            int get_sock();
    };
}


#endif