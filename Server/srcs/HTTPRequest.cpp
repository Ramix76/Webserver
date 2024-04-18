#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest(const std::string& request) {
    // Analizar la solicitud HTTP
    size_t pos = request.find("\r\n\r\n"); // Encuentra el final de los encabezados
    if (pos != std::string::npos) {
        // Extraer la parte de los encabezados
        std::string headerPart = request.substr(0, pos);
        
        // Separar la solicitud en líneas
        std::istringstream iss(headerPart);
        std::string line;
        bool firstLine = true;
        while (std::getline(iss, line, '\n')) {
            // Eliminar retorno de carro y nueva línea
            line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
            line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
            
            if (firstLine) {
                // Analizar la primera línea para obtener el método y la ruta
                std::istringstream firstLineStream(line);
                std::string method, path;
                firstLineStream >> method >> path;
                this->method = method;
                this->path = path;
                firstLine = false;
            } else {
                // Separar cada línea de encabezado en clave y valor
                size_t separatorPos = line.find(": ");
                if (separatorPos != std::string::npos) {
                    std::string key = line.substr(0, separatorPos);
                    std::string value = line.substr(separatorPos + 2);
                    headers[key] = value;
                }
            }
        }
    }
}

std::string HTTPRequest::getHeader(const std::string& key) const {
    // Obtener un encabezado específico de la solicitud
    std::map<std::string, std::string>::const_iterator it = headers.find(key);
    return (it != headers.end()) ? it->second : "";
}