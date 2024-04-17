#include "BindingSocket.hpp"

FRP::BindingSocket::~BindingSocket()
{
    // Llama al destructor de SimpleSocket para cerrar el socket
    SimpleSocket::~SimpleSocket();
}

// Constructor
FRP::BindingSocket::BindingSocket(int domain, int service, int protocol, int port, u_long interface) : SimpleSocket(domain, service, protocol, port, interface)
{
    // Connect to the network and confirm success
    // Pass parameters to parent constructor for instantiation
    connect_to_network(get_sock(), get_address());
    test_connection(binding);
}

// Mutators
void FRP::BindingSocket::connect_to_network(int sock, struct sockaddr_in address)
{
    // Establish the binding connection to the network
    binding = bind(sock, (struct sockaddr *)&address, sizeof(address));
}

// Getters
int FRP::BindingSocket::get_binding()
{
    return (binding);
}