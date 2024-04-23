#include "SocketServer.hpp"
#include "HTTPRequest.hpp"
#include "HandleRequestWrapper.hpp"

SocketServer::SocketServer(const std::string& configFile)
    : logger("server.log") // Inicializa el objeto Logger con el nombre del archivo de registro
{
    // Leer y analizar el archivo de configuración
    
    // Log que se está leyendo y analizando el archivo de configuración
    logger.log("Reading and parsing the configuration file...");

    std::ifstream file(configFile);
    if (file.is_open())
    {
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
                }
                else if (key == "server_name")
                {
                    config.serverName = value;
                }
                else if (key == "error_page_404")
                {
                    config.errorPage404 = value;
                }
                // Agrega más opciones de configuración según sea necesario
            }
        }
        file.close();
    }
    else
    {
        // Si no se puede abrir el archivo, imprimir un mensaje de error
        std::cerr << "Error: Unable to open configuration file" << std::endl;
        // Salir del programa o manejar el error según sea necesario
        exit(EXIT_FAILURE);
    }
    // Crear un socket

    // Log que se está creando el socket
    logger.log("Creating a socket...");

    listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1)
    {
        perror("Can't create a socket");
        exit(EXIT_FAILURE);
    }

    // Set SO_REUSEADDR option to allow socket reuse
    int opt = 1;
    if (setsockopt(listening, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "Error: Can't set SO_REUSEADDR option. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    // Bind the socket to a IP/port

    // Log que se está vinculando el socket a una IP/puerto
    logger.log("Binding the socket to IP/port...");

    memset(&hint, 0, sizeof(hint));
    hint.sin_family = AF_INET;
    hint.sin_port = htons(config.port); // Usar el puerto configurado
    hint.sin_addr.s_addr = INADDR_ANY;

    if (bind(listening, (struct sockaddr *)&hint, sizeof(hint)) == -1)
    {
        perror("Can't bind to IP/port");
        exit(EXIT_FAILURE);
    }

    // Mark the socket for listening in
    if (listen(listening, SOMAXCONN) == -1)
    {
        perror("Can't listen");
        exit(EXIT_FAILURE);
    }

    // Log que el servidor ha comenzado
    logger.log("Server has started.");
}

SocketServer::~SocketServer()
{
    // Cerrar el archivo de registro
    logFile.close();

    // Close client socket
    close(clientSocket);

    // Close listening socket
    close(listening);
}

void SocketServer::Start() {
    // Iniciar el servidor y manejar las solicitudes entrantes

    // Log que el servidor ha iniciado y está esperando conexiones entrantes
    logger.log("Server started and waiting for incoming connections...");

    while (true) {
        // Accept a call

        // Log que se ha aceptado una conexión de cliente
        logger.log("Accepted a connection from a client.");

        clientSize = sizeof(client);
        clientSocket = accept(listening, (struct sockaddr *)&client, &clientSize);
        if (clientSocket == -1) {
            perror("Problem with client connecting");
            continue; // Continue accepting connections
        }

        // Print client information
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client.sin_addr, clientIP, INET_ADDRSTRLEN);
        std::cout << "Client connected: " << clientIP << ":" << ntohs(client.sin_port) << std::endl;

        // Create a new thread to handle the request using a function object
        
        // Log que se ha creado un nuevo hilo para manejar la solicitud del cliente
        logger.log("Created a new thread to handle the client request.");

        HandleRequestWrapper wrapper(this, clientSocket);
        std::thread clientThread(wrapper);
        clientThread.detach(); // Detach the thread to run independently
    }
}

void SocketServer::sendResponse(int clientSocket, const std::string& response) {
    // Envía la respuesta al cliente

    // Log que se está enviando una respuesta al cliente
    logger.log("Sending response to client...");
    std::cout << "Sending response to client." << std::endl;

    ssize_t bytesSent = send(clientSocket, response.c_str(), response.size(), 0);
    if (bytesSent == -1) {
        perror("Error sending response");
    } else {
        // Envía un acknowledgment al cliente
        const std::string acknowledgment = " ACK "; // Puedes personalizar este mensaje según tus necesidades
        ssize_t ackSent = send(clientSocket, acknowledgment.c_str(), acknowledgment.size(), 0);
        if (ackSent == -1) {
            // Log que el servidor ha enviado un ACK al cliente
            logger.log("Sent ACK to client");
            perror("Error sending acknowledgment");
        }
    }
}

void SocketServer::handleRequest(int clientSocket) {
    // Log the start of handling a request
    logger.log("Handling a new request...");

    // While loop to handle multiple requests on the same connection
    while (true) {
        // Receive the request from the client
        logger.log("Received a request from client.");
        std::cout << "Received a request from client." << std::endl;
        char buffer[4096];
        ssize_t bytesRecv = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRecv <= 0) {
            if (bytesRecv == 0) {
                std::cout << "The client disconnected" << std::endl;
                logger.log("The client disconnected");
            } else {
                perror("There was a connection issue");
                logger.log("There was a connection issue");
            }
            
            close(clientSocket);
            return;
        }

        // Process the received request
        std::string request(buffer, bytesRecv);
        handleHTTPRequest(request, clientSocket);
        logger.log("Processed client request." + request);
        std::cout << "Processed client request: " <<  request << std::endl;
    }
}

void SocketServer::handleHTTPRequest(const std::string& request, int clientSocket) {
    // Parse the request to extract method, path, headers, etc.
    // For simplicity, let's assume the request format is:
    // METHOD PATH HTTP_VERSION\r\n
    // Header1: Value1\r\n
    // Header2: Value2\r\n
    // \r\n
    // Request body (if applicable)

    // Log the received message from the client
    logger.log("Received HTTP request from client: " + request);
    std::cout << "Received HTTP request from client: " << request <<  std::endl;

    // Aquí puedes procesar la solicitud y generar la respuesta apropiada
    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, dear client!";
    
    // Envía la respuesta al cliente
    sendResponse(clientSocket, response);

    // Extract method and path
    std::istringstream iss(request);
    std::string method, path, httpVersion;
    iss >> method >> path >> httpVersion;

    // Process the request based on the method and path
    if (method == "GET") {
        handleGETRequest(path, clientSocket);
    } else if (method == "POST") {
        handlePOSTRequest(path, clientSocket, request);
    } else if (method == "DELETE") {
        handleDELETERequest(path, clientSocket);
    } else {
        sendHTTPResponse(clientSocket, "405 Method Not Allowed", "Method not allowed", 405);
    }
    logger.log("Processed HTTP request from client.");
    std::cout << "Processed HTTP request from client." << std::endl;
}

void SocketServer::handleGETRequest(const std::string& /*path*/, int /*clientSocket*/) {
    // Handle GET request logic here
    // Serve static files, generate dynamic content, etc.
    // Log the received HTTP request
    // logger.log("Handling a GET request: " + request);
}

void SocketServer::handlePOSTRequest(const std::string& /*path*/, int /*clientSocket*/, const std::string& /*request*/) {
    // Handle POST request logic here
    // Process form data, upload files, etc.
    // Log the received HTTP request
    // logger.log("Handling a POST request: " + request);
}

void SocketServer::handleDELETERequest(const std::string& /*path*/, int /*clientSocket*/) {
    // Handle DELETE request logic here
    // Delete files, resources, etc.
    // logger.log("Handling a DELETE request: " + request);
}

void SocketServer::sendHTTPResponse(int clientSocket, const std::string& status, const std::string& content, int statusCode) {
    // Construct and send the HTTP response
    std::ostringstream oss;
    oss << "HTTP/1.1 " << statusCode << " " << status << "\r\n";
    oss << "Content-Length: " << content.length() << "\r\n";
    oss << "\r\n";
    oss << content;

    std::string response = oss.str();
    send(clientSocket, response.c_str(), response.length(), 0);
}