#include "SocketServer.hpp"
#include "HTTPRequest.hpp"

SocketServer::SocketServer(const std::string& configFile) {
    // Leer y analizar el archivo de configuración
    std::ifstream file(configFile);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string key, value;
            if (std::getline(iss, key, '=') && std::getline(iss, value)) {
                // Analizar y configurar las propiedades del servidor según las líneas del archivo
                if (key == "port") {
                    config.port = std::stoi(value);
                } else if (key == "server_name") {
                    config.serverName = value;
                } else if (key == "error_page_404") {
                    config.errorPage404 = value;
                }
                // Agrega más opciones de configuración según sea necesario
            }
        }
        file.close();
    } else {
        // Si no se puede abrir el archivo, imprimir un mensaje de error
        std::cerr << "Error: Unable to open configuration file" << std::endl;
        // Salir del programa o manejar el error según sea necesario
        exit(EXIT_FAILURE);
    }
    // Crear un socket
    listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1) {
        perror("Can't create a socket");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to a IP/port
    memset(&hint, 0, sizeof(hint));
    hint.sin_family = AF_INET;
    hint.sin_port = htons(config.port); // Usar el puerto configurado
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

void SocketServer::handleRequest(const std::string& request) {
    // Handle the request based on HTTP method
    HTTPRequest httpRequest(request);
    
    // Obtener el método HTTP
    std::string method = httpRequest.getMethod();

    // Obtener la ruta solicitada
    std::string path = httpRequest.getPath();

    // Imprimir el método y la ruta
    std::cout << "Method: " << method << std::endl;
    std::cout << "Path: " << path << std::endl;

    // Implementar la lógica para manejar diferentes métodos HTTP, como GET, POST, DELETE, etc.
    if (method == "GET") {
        // Implementar la lógica para manejar la solicitud GET
    } else if (method == "POST") {
        // Implementar la lógica para manejar la solicitud POST
    } else if (method == "DELETE") {
        // Implementar la lógica para manejar la solicitud DELETE
    } else {
        // Método HTTP no compatible, devolver un error
        // Implementar la lógica para manejar otros métodos HTTP
    }
}