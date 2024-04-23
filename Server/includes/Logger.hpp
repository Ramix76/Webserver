#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

class Logger {
public:
    Logger(); // Constructor predeterminado
    Logger(const std::string& filename); // Constructor para inicializar el archivo de registro
    ~Logger(); // Destructor

    void log(const std::string& message); // Método para registrar un mensaje en el archivo de registro

private:
    std::ofstream logfile; // Archivo de registro
};

#endif /* LOGGER_HPP */