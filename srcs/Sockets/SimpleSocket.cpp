#include "SimpleSocket.hpp"

FRP::SimpleSocket::SimpleSocket(int domain, int service, int protocol, int port, u_long interface)
{
    //Define address structure
    address.sin_family = domain;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(interface);
    //Estabish socket
    sock = socket(domain, service, protocol);
    test_connection(sock);
}

//Test connection virtual function

void FRP::SimpleSocket::test_connection(int item_to_test)
{
    if (item_to_test < 0)
    {
        perror("Failed to connect...");
        exit(EXIT_FAILURE);
    }
}

//Getter functions

struct sockaddr_in FRP::SimpleSocket::get_address()
{
    return (address);
}

int FRP::SimpleSocket::get_sock()
{
    return (sock);
}