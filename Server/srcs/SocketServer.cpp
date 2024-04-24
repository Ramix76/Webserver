#include "SocketServer.hpp"
#include "HTTPRequest.hpp"

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
                else if (key == "static_html_file")
                {
                    config.staticHtmlFile = value;
                    logger.log("Set static HTML file to: " + value);
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

    // Extract method and path from the request
    std::istringstream requestStream(request);
    std::string method, path;
    requestStream >> method >> path;

    // Construct full path to static HTML file
    std::string filePath = config.staticHtmlFile;
    logger.log("Full path to static HTML file: " + filePath);

    // Check if the request is for the static HTML file
    if (method == "GET" && path == "/") {
        // Open the static HTML file
        std::ifstream file(filePath.c_str());
        std::string response;

        if (file.is_open()) {
            // Read the content of the file into the response
            std::stringstream buffer;
            buffer << file.rdbuf();
            response = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(buffer.str().length()) + "\r\n\r\n" + buffer.str();
            logger.log("Static HTML file loaded successfully");
        } else {
            // If the file cannot be opened, respond with a 404 error
            response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
            logger.log("Error: Static HTML file not found");
        }

        // Send the response to the client
        sendResponse(clientSocket, response);
    } else {
        // If the request is not for the static HTML file, respond with a 404 error
        std::string response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
        logger.log("Error: Request is not for the static HTML file");
        sendResponse(clientSocket, response);
    }
}

void SocketServer::sendResponse(int clientSocket, const std::string& response)
{
    // Envía la respuesta al cliente
    ssize_t bytesSent = send(clientSocket, response.c_str(), response.size(), 0);
    if (bytesSent == -1) {
        perror("Error sending response");
    }
}

std::string SocketServer::getMimeType(const std::string& filePath) {
    // Obtiene el tipo MIME del archivo basado en su extensión
    std::string::size_type pos = filePath.rfind('.');
    if (pos != std::string::npos) {
        std::string ext = filePath.substr(pos + 1);
        if (ext == "html" || ext == "htm") {
            return "text/html";
        } else if (ext == "css") {
            return "text/css";
        } else if (ext == "js") {
            return "text/javascript";
        } else if (ext == "jpg" || ext == "jpeg") {
            return "image/jpeg";
        } else if (ext == "png") {
            return "image/png";
        } else if (ext == "gif") {
            return "image/gif";
        } else {
            return "application/octet-stream"; // Tipo MIME predeterminado para archivos desconocidos
        }
    }
    return "application/octet-stream"; // Tipo MIME predeterminado si no se puede determinar la extensión
}

std::string SocketServer::readFile(const std::string& filePath) {
    // Lee el contenido del archivo y lo devuelve como una cadena
    std::ifstream file(filePath.c_str(), std::ios::binary);
    if (file) {
        std::ostringstream ss;
        ss << file.rdbuf();
        return ss.str();
    } else {
        return ""; // Devuelve una cadena vacía si hay algún error al leer el archivo
    }
}