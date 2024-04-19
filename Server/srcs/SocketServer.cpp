#include "SocketServer.hpp"
#include "HTTPRequest.hpp"
#include "HandleRequestWrapper.hpp"

SocketServer::SocketServer(const std::string& configFile)
{
    // Abrir el archivo de registro
    logFile.open("server.log", std::ios::out | std::ios::app);
    if (!logFile.is_open())
    {
        std::cerr << "Error: Unable to open log file. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        std::cout << "Log file opened successfully" << std::endl; // Mensaje de depuración
        logFile << "Log file opened successfully" << std::endl;
        logFile.flush(); // Flush buffer to ensure data is written to the file
    }

    // Leer y analizar el archivo de configuración
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
    std::cout << "Server listening for connections..." << std::endl;
    logFile << "Server listening for connections..." << std::endl;
    logFile.flush(); // Flush buffer to ensure data is written to the file
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
    while (true) {
        // Accept a call
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
        HandleRequestWrapper wrapper(this, clientSocket);
        std::thread clientThread(wrapper);
        clientThread.detach(); // Detach the thread to run independently
    }
}

void SocketServer::sendResponse(int clientSocket, const std::string& response) {
    // Envía la respuesta al cliente
    ssize_t bytesSent = send(clientSocket, response.c_str(), response.size(), 0);
    if (bytesSent == -1) {
        perror("Error sending response");
    }
}

void SocketServer::handleRequest(int clientSocket) {
    // While loop to handle multiple requests on the same connection
    while (true) {
        // Receive the request from the client
        char buffer[4096];
        ssize_t bytesRecv = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRecv <= 0) {
            if (bytesRecv == 0) {
                std::cout << "The client disconnected" << std::endl;
            } else {
                perror("There was a connection issue");
            }
            
            close(clientSocket);
            return;
        }

        // Process the received request
        std::string request(buffer, bytesRecv);
        handleHTTPRequest(request, clientSocket);
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
    std::cout << "Received message from client: " << request << std::endl;

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
}

void SocketServer::handleGETRequest(const std::string& /*path*/, int /*clientSocket*/) {
    // Handle GET request logic here
    // Serve static files, generate dynamic content, etc.
}

void SocketServer::handlePOSTRequest(const std::string& /*path*/, int /*clientSocket*/, const std::string& /*request*/) {
    // Handle POST request logic here
    // Process form data, upload files, etc.
}

void SocketServer::handleDELETERequest(const std::string& /*path*/, int /*clientSocket*/) {
    // Handle DELETE request logic here
    // Delete files, resources, etc.
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

// void SocketServer::Start() {
//     while (true) {
//         // Accept a call
//         clientSize = sizeof(client);
//         clientSocket = accept(listening, (struct sockaddr *)&client, &clientSize);
//         if (clientSocket == -1) {
//             perror("Problem with client connecting");
//             exit(EXIT_FAILURE);
//         }

//         // Print client information
//         char clientIP[INET_ADDRSTRLEN];
//         inet_ntop(AF_INET, &client.sin_addr, clientIP, INET_ADDRSTRLEN);
//         std::cout << "Client connected: " << clientIP << ":" << ntohs(client.sin_port) << std::endl;

//         // Receive request from client
//         char buffer[4096];
//         ssize_t bytesRecv;
//         while ((bytesRecv = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
//             // Display the received message
//             std::cout << "Received from client: " << buffer << std::endl;

//             // Handle the request
//             // handleRequest(std::string(buffer, bytesRecv));
//             handleRequest(buffer);

//             // Resend the message
//             send(clientSocket, buffer, bytesRecv, 0);
//         }

//         // Check for errors or client disconnect
//         if (bytesRecv == 0) {
//             std::cout << "The client disconnected" << std::endl;
//         } else {
//             std::cerr << "Error: There was a connection issue. errno: " << errno << std::endl;
//         }

//         // Close client socket
//         close(clientSocket);
//     }
// }

// void SocketServer::handleRequest(const std::string& request) {
//     // Handle the request based on HTTP method
//     HTTPRequest httpRequest(request);
    
//     // Obtener el método HTTP
//     std::string method = httpRequest.getMethod();

//     // Obtener la ruta solicitada
//     std::string path = httpRequest.getPath();

//     // Log the request
//     std::ofstream logFile("server.log", std::ios::out | std::ios::app);
//     if (logFile.is_open()) {
//         logFile << "Received request - Method: " << method << ", Path: " << path << std::endl;
//         logFile.close();
//     } else {
//         std::cerr << "Error: Unable to open log file" << std::endl;
//     }

//     // Generate HTTP response
//     std::string response = getHTTPResponse(method, path);

//     // Send the response
//     send(clientSocket, response.c_str(), response.size(), 0);
// }


// std::string SocketServer::getHTTPResponse(const std::string& method, const std::string& path) {
//     // Handle GET method
//     if (method == "GET") {
//         std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
//         if (file) {
//             std::ostringstream contents;
//             contents << file.rdbuf();
//             file.close();
//             std::string content = contents.str();

//             // Generate HTTP response
//             std::ostringstream response;
//             response << "HTTP/1.1 200 OK\r\n";
//             response << "Content-Length: " << content.size() << "\r\n";
//             response << "\r\n";
//             response << content;
//             return response.str();
//         } else {
//             // File not found, generate 404 response
//             std::string notFoundContent = "404 Not Found";
//             std::ostringstream response;
//             response << "HTTP/1.1 404 Not Found\r\n";
//             response << "Content-Length: " << notFoundContent.size() << "\r\n";
//             response << "\r\n";
//             response << notFoundContent;
//             return response.str();
//         }
//     } else if (method == "POST" || method == "DELETE") {
//         // Handle other methods (POST and DELETE)
//         // You can add specific logic for these methods here
//         // For now, return 405 Method Not Allowed
//         std::string methodNotAllowedContent = "405 Method Not Allowed";
//         std::ostringstream response;
//         response << "HTTP/1.1 405 Method Not Allowed\r\n";
//         response << "Content-Length: " << methodNotAllowedContent.size() << "\r\n";
//         response << "\r\n";
//         response << methodNotAllowedContent;
//         return response.str();
//     } else {
//         // Unsupported HTTP method, generate 405 response
//         std::string methodNotAllowedContent = "405 Method Not Allowed";
//         std::ostringstream response;
//         response << "HTTP/1.1 405 Method Not Allowed\r\n";
//         response << "Content-Length: " << methodNotAllowedContent.size() << "\r\n";
//         response << "\r\n";
//         response << methodNotAllowedContent;
//         return response.str();
//     }
// }

// void SocketServer::Start()
// {
//     // Accept a call
//     clientSize = sizeof(client);
//     clientSocket = accept(listening, (struct sockaddr *)&client, &clientSize);
//     if (clientSocket == -1)
//     {
//         perror("Problem with client connecting");
//         exit(EXIT_FAILURE);
//     }

//     // Print client information
//     char clientIP[INET_ADDRSTRLEN];
//     inet_ntop(AF_INET, &client.sin_addr, clientIP, INET_ADDRSTRLEN);
//     std::cout << "Client connected: " << clientIP << ":" << ntohs(client.sin_port) << std::endl;

//     // Write client connection information to log file
//     logFile << "Client connected: " << clientIP << ":" << ntohs(client.sin_port) << std::endl;
//     logFile.flush(); // Flush buffer to ensure data is written to the file

//     // While receiving- display message, echo message
//     char buffer[4096];
//     ssize_t bytesRecv;
//     while ((bytesRecv = read(clientSocket, buffer, sizeof(buffer))) > 0)
//     {
//         // Display the received message
//         std::cout << "Received from client: " << buffer << std::endl;
//         logFile << "Received from client: " << buffer << std::endl;
//         logFile.flush(); // Flush buffer to ensure data is written to the file
//         // Resend the message
//         send(clientSocket, buffer, bytesRecv, 0);
//     }

//     // Check for errors or client disconnect
//     if (bytesRecv == 0)
//     {
//         std::cout << "The client disconnected" << std::endl;
//         logFile << "The client disconnected" << std::endl;
//     }
//     else
//     {
//         perror("There was a connection issue");
//         logFile << "There was a connection issue" << std::endl;
//     }
// }

// void SocketServer::handleRequest(const std::string& request)
// {
//     // Handle the request based on HTTP method
//     HTTPRequest httpRequest(request);
    
//     // Obtener el método HTTP
//     std::string method = httpRequest.getMethod();

//     // Obtener la ruta solicitada
//     std::string path = httpRequest.getPath();

//     // Write debug information to log file
//     logFile << "Received request - Method: " << method << ", Path: " << path << std::endl;
//     logFile.flush(); // Flush buffer to ensure data is written to the file

//     // Imprimir el método y la ruta
//     std::cout << "Method: " << method << std::endl;
//     std::cout << "Path: " << path << std::endl;

//     // Implementar la lógica para manejar diferentes métodos HTTP, como GET, POST, DELETE, etc.
//     if (method == "GET")
//     {
//         // Implementar la lógica para manejar la solicitud GET
//     }
//     else if (method == "POST")
//     {
//         // Implementar la lógica para manejar la solicitud POST
//     }
//     else if (method == "DELETE")
//     {
//         // Implementar la lógica para manejar la solicitud DELETE
//     }
//     else
//     {
//         // Método HTTP no compatible, devolver un error
//         // Implementar la lógica para manejar otros métodos HTTP
//     }
// }