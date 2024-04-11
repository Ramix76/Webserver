#include "SimpleServer.hpp"

FRP::SimpleServer::SimpleServer(int domain, int service, int protocol, int port, u_long interface, int bklg)
{
    socket = new ListeningSocket(domain, service, protocol, port, interface, bklg); 
}

FRP::ListeningSocket *FRP::SimpleServer::get_socket()
{
    return (socket);
}