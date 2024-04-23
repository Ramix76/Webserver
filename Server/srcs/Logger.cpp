#include "Logger.hpp"

Logger::Logger(const std::string& filename) {
    // Abre el archivo de registro en modo de escritura, truncando el archivo si ya existe

    logfile.open("server.log", std::ios::out | std::ios::trunc);
    if (!logfile.is_open()) {
        std::cerr << "Error: Unable to open log file server.log" << filename << std::endl;
        // Puedes manejar este error de alguna otra manera si es necesario
    } else {
        // Log que se está abriendo el archivo de registro
        logfile << "Opening log file..." << std::endl;
    }
}

Logger::~Logger() {
    // Cierra el archivo de registro

    if (logfile.is_open()) {
        // Log que se está cerrando el archivo de registro
        logfile << "Closing log file..." << std::endl;
        logfile.close();
    }
}

void Logger::log(const std::string& message) {
    // Obtiene la fecha y hora actual
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);

    // Formatea la fecha y hora en una cadena legible
    char timeBuffer[80];
    std::strftime(timeBuffer, sizeof(timeBuffer), "[%Y-%m-%d %H:%M:%S] ", localTime);

    // Escribe el mensaje de registro junto con la marca de tiempo en el archivo de registro
    if (logfile.is_open()) {
        logfile << timeBuffer << message << std::endl;
    } else {
        std::cerr << "Error: Log file is not open" << std::endl;
        // Puedes manejar este error de alguna otra manera si es necesario
    }
}