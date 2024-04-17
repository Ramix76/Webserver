#include "ConnectingSocket.hpp"

FRP::ConnectingSocket::~ConnectingSocket()
{
    // Llama al destructor de SimpleSocket para cerrar el socket
    SimpleSocket::~SimpleSocket();
}

// Constructor
FRP::ConnectingSocket::ConnectingSocket(int domain, int service, int protocol, int port, u_long interface) : SimpleSocket(domain, service, protocol, port, interface)
{
    connect_to_network(get_sock(), get_address());
    test_connection(get_connection());
}

// Definition of the connect_to_network virtual function

void FRP::ConnectingSocket::connect_to_network(int sock, struct sockaddr_in address)
{
    connection = connect(sock, (struct sockaddr *)&address, sizeof(address));
}