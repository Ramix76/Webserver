#include "ListeningSocket.hpp"

int main()
{
    std::cout << "Starting..." << std::endl;
        // std::cout << "Simple socket..." << std::end;
        // FRP::SimpleSocket ss = FRP::SimpleSocket(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY);
    std::cout << "Binding Socket..." << std::endl;
    FRP::BindingSocket bs = FRP::BindingSocket(AF_INET, SOCK_STREAM, 0, 81 , INADDR_ANY);
    std::cout << "Listening Socket..." << std::endl;
    FRP::ListeningSocket ls = FRP::ListeningSocket(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 10);
    std::cout << "Succes!" << std::endl;
}