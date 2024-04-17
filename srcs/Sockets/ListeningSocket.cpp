#include "ListeningSocket.hpp"

FRP::ListeningSocket::~ListeningSocket()
{
    // Llama al destructor de SimpleSocket para cerrar el socket
    SimpleSocket::~SimpleSocket();
}

// Constructor
FRP::ListeningSocket::ListeningSocket(int domain, int service, int protocol, int port, u_long interface, int bklg) : BindingSocket(domain, service, protocol, port, interface)
{
    // Set baclog variable
    backlog = bklg;
    // Start listening to the network using listen() from sys/socket.h
    start_listening();
    // Corfim the connection was successful
    test_connection(listening);
}

// Mutators
void FRP::ListeningSocket::start_listening()
{
    // Start listening on the network
    listening = listen(get_sock(), backlog);
}

// Getters
int FRP::ListeningSocket::get_listening()
{
    return (listening);
}

int FRP::ListeningSocket::get_backlog()
{
    return (backlog);
}