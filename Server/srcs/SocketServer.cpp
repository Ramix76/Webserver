#include "SocketServer.hpp"
// #include "HTTPRequest.hpp"

SocketServer::SocketServer(const std::string& configFile, Logger &logger) : logger(logger)
{
    logger.log("Starting the server...");
    // Abrir el archivo de configuración
    std::ifstream file(configFile);
    logger.log("Opening configuration file: " + configFile);
    if (file.is_open())
    {
        logger.log("Successfully opened configuration file: " + configFile);
        std::string line;
        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::string key, value;
            if (std::getline(iss, key, '=') && std::getline(iss, value))
            {
                // Analizar y configurar las propiedades del servidor según las líneas del archivo
                if (key == "port")
                {
                    config.port = std::stoi(value);
                    logger.log("Set port to: " + value);
                }
                else if (key == "server_name")
                {
                    config.serverName = value;
                    logger.log("Set server name to: " + value);
                }
                else if (key == "error_page_404")
                {
                    config.errorPage404 = value;
                    logger.log("Set 404 error page to: " + value);
                }
                // Agrega más opciones de configuración según sea necesario
            }
        }
        file.close();
    }
    else
    {
        // Si no se puede abrir el archivo, registrar un mensaje de error
        logger.log("Error: Unable to open configuration file: " + configFile);
        // Salir del programa o manejar el error según sea necesario
        exit(EXIT_FAILURE);
    }
    
    // Crear el socket de escucha
    // Configurar el socket
    // Inicializar kqueue y registrar el socket de escucha en él

    // Crear un socket de escucha
    listening = socket(AF_INET, SOCK_STREAM, 0);
    logger.log("Creating listening socket...");
    if (listening == -1) {
        perror("Can't create a socket");
        exit(EXIT_FAILURE);
    }

    // Configurar el socket
    int opt = 1;
    if (setsockopt(listening, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "Error: Can't set SO_REUSEADDR option. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    // Bind the socket to a IP/port
    struct sockaddr_in hint;
    memset(&hint, 0, sizeof(hint));
    hint.sin_family = AF_INET;
    hint.sin_port = htons(config.port); // Usar el puerto configurado
    hint.sin_addr.s_addr = INADDR_ANY;
    logger.log("Binding socket to port: " + std::to_string(config.port));
    if (bind(listening, (struct sockaddr *)&hint, sizeof(hint)) == -1) {
        perror("Can't bind to IP/port");
        exit(EXIT_FAILURE);
    }

    // Mark the socket for listening in
    if (listen(listening, SOMAXCONN) == -1) {
        perror("Can't listen");
        exit(EXIT_FAILURE);
    }

    // Inicializar kqueue
    kq = kqueue();
    logger.log("Initializing kqueue...");
    if (kq == -1) {
        perror("Error creating kqueue");
        exit(EXIT_FAILURE);
    }

    // Agregar el socket de escucha al kqueue
    struct kevent event;
    EV_SET(&event, listening, EVFILT_READ, EV_ADD, 0, 0, NULL);
    logger.log("Adding listening socket to kqueue...");
    if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
        perror("Error adding listening socket to kqueue");
        exit(EXIT_FAILURE);
    }
}

SocketServer::~SocketServer()
{
    // Cerrar el socket de escucha y liberar recursos
    close(listening);
}

void SocketServer::Start()
{
    // Registrar un mensaje indicando que el servidor ha comenzado
    logger.log("The server is starting...");

    // Bucle principal para manejar eventos con kqueue
    while (true) {
        // Monitorear eventos en los sockets usando kqueue
        struct kevent events[MAX_EVENTS];
        int numEvents = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
        if (numEvents == -1) {
            perror("Error in kevent");
            logger.log("Error in kevent");
            exit(EXIT_FAILURE);
        }

        // Procesar los eventos
        for (int i = 0; i < numEvents; ++i) {
            int fd = events[i].ident;
            if (fd == listening) {
                // Evento en el socket de escucha, aceptar una nueva conexión
                acceptConnection();
            } else {
                // Evento en un socket de cliente, manejar la solicitud
                handleRequest(fd);
            }
        }
    }
}

void SocketServer::acceptConnection()
{
    // Aceptar una nueva conexión y agregar el socket del cliente al kqueue
    struct sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    int clientSocket = accept(listening, (struct sockaddr *)&client, &clientSize);
    if (clientSocket == -1) {
        perror("Problem with client connecting");
        return; // Continue accepting connections
    }

    // Agregar el socket del cliente al kqueue
    struct kevent event;
    EV_SET(&event, clientSocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
        perror("Error adding client socket to kqueue");
        close(clientSocket);
        return;
    }
}

void SocketServer::handleRequest(int clientSocket)
{
    // Log that a client request is being handled
    logger.log("Handling client request...");

    // Receive the request from the client
    char buffer[4096];
    ssize_t bytesRecv = recv(clientSocket, buffer, sizeof(buffer), 0);

    // Check for connection issues or disconnection
    if (bytesRecv <= 0) {
        if (bytesRecv == 0) {
            logger.log("The client disconnected");
        } else {
            perror("There was a connection issue");
            logger.log("There was a connection issue");
        }
        
        close(clientSocket);
        return;
    }

    // Process the received request
    logger.log("Received request from client:");

    // Print the received request
    std::string request(buffer, bytesRecv);
    logger.log("Request: " + request);

    // Send a response to the client
    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 19\r\n\r\nHello, dear client!";
    logger.log("Sending response to client:");
    logger.log(response);

    sendResponse(clientSocket, response);
}

void SocketServer::sendResponse(int clientSocket, const std::string& response)
{
    // Envía la respuesta al cliente
    ssize_t bytesSent = send(clientSocket, response.c_str(), response.size(), 0);
    if (bytesSent == -1) {
        perror("Error sending response");
    }
}