#include "TestServer.hpp"
#include "ConnectingSocket.hpp"
#include <chrono>
#include <thread>

FRP::TestServer::TestServer() : SimpleServer(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 10)
{
    launch();
}

void FRP::TestServer::accepter()
{
    struct sockaddr_in address = get_socket()->get_address();
    int addrlen = sizeof(address);
    new_socket = accept(get_socket()->get_sock(), (struct sockaddr *)&address, (socklen_t *)&addrlen);
    read(new_socket, buffer, 30000);
}

void FRP::TestServer::handler()
{
    std::cout << "Request received:\n" << buffer << std::endl;
}

void FRP::TestServer::responder()
{
    const char *response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/plain\r\n"
                           "Content-Length: 14\r\n"
                           "\r\n"
                           "Hello, world!\r\n";
    
    write(new_socket, response, strlen(response));
}

void FRP::TestServer::launch()
{
    std::cout << "====== WAITING... =======" << std::endl;
    // Crear una instancia de ConnectingSocket para manejar las conexiones entrantes
    FRP::ConnectingSocket server_socket(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY);
    while (true)
    {
        if (server_socket.get_connection() >= 0)
        {
            std::cout << "Connection established successfully!" << std::endl;
            accepter();
            handler();
            responder();
            std::cout << "======= DONE! =========" << std::endl;
        }
        else
            std::cerr << "Failed to establish connection!" << std::endl;
    }
}