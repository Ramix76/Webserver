#include "SocketServer.hpp"

SocketServer::SocketServer(int port) {
    // Create a socket
    listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1) {
        perror("Can't create a socket");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to a IP/port
    memset(&hint, 0, sizeof(hint));
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    hint.sin_addr.s_addr = INADDR_ANY;

    if (bind(listening, (struct sockaddr *)&hint, sizeof(hint)) == -1) {
        perror("Can't bind to IP/port");
        exit(EXIT_FAILURE);
    }

    // Mark the socket for listening in
    if (listen(listening, SOMAXCONN) == -1) {
        perror("Can't listen");
        exit(EXIT_FAILURE);
    }
}

SocketServer::~SocketServer() {
    // Close client socket
    close(clientSocket);

    // Close listening socket
    close(listening);
}

void SocketServer::Start() {
    // Accept a call
    clientSize = sizeof(client);
    clientSocket = accept(listening, (struct sockaddr *)&client, &clientSize);
    if (clientSocket == -1) {
        perror("Problem with client connecting");
        exit(EXIT_FAILURE);
    }

    // Print client information
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client.sin_addr, clientIP, INET_ADDRSTRLEN);
    std::cout << "Client connected: " << clientIP << ":" << ntohs(client.sin_port) << std::endl;

    // While receiving- display message, echo message
    char buffer[4096];
    ssize_t bytesRecv;
    while ((bytesRecv = read(clientSocket, buffer, sizeof(buffer))) > 0) {
        // Display the received message
        std::cout << "Received from client: " << buffer << std::endl;
        // Resend the message
        send(clientSocket, buffer, bytesRecv, 0);
    }

    // Check for errors or client disconnect
    if (bytesRecv == 0) {
        std::cout << "The client disconnected" << std::endl;
    } else {
        perror("There was a connection issue");
    }
}

// void SocketServer::Start() {
//     // Accept a call
//     clientSize = sizeof(client);
//     clientSocket = accept(listening, (struct sockaddr *)&client, &clientSize);
//     if (clientSocket == -1) {
//         perror("Problem with client connecting");
//         exit(EXIT_FAILURE);
//     }

//     // Convert IP address from network byte order to string
//     char *ip = inet_ntoa(client.sin_addr);

//     // Print client information
//     std::cout << "Client connected: " << ip << ":" << ntohs(client.sin_port);

//     // While receiving- display message, echo message
//     char buffer[4096];
//     while (true) {
//         // Clear the buffer
//         memset(buffer, 0, sizeof(buffer));
//         // Wait for a message
//         int bytesRecv = recv(clientSocket, buffer, sizeof(buffer), 0);
//         if (bytesRecv == -1) {
//             perror("There was a connection issue");
//             break;
//         }
//         if (bytesRecv == 0) {
//             std::cout << "The client disconnected" << std::endl;
//             break;
//         }
//         // Display the message
//         std::cout << "Received: " << std::string(buffer, 0, bytesRecv);
//         // Resend the message
//         send(clientSocket, buffer, bytesRecv, 0);
//     }
// }