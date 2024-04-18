#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string>
#include <cstring> // For memset
#include <cerrno> // For errno

using namespace std;

int main()
{
    // Create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        return -1;

    // Create a hint structure for the server we are connecting with
    int port = 54000;
    string ipAddress = "127.0.0.1";
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

    // Connect to the server on the socket
    int connectRes = connect(sock, (sockaddr *)&hint, sizeof(hint));
    if (connectRes == -1)
        return (-1);

    // While loop:
    char buffer[4096];
    string userInput;
    do {
        // Enter lines of text
        cout << "> ";
        getline(cin, userInput);
        // Send to server
        int sendRes = send(sock, userInput.c_str(), userInput.size() + 1, 0);
        if (sendRes == -1)
            return (-1);
        // Wait for response
        memset(buffer, 0, 4096);
        int bytesRcv = recv(sock, buffer, 4096, 0);
        // Display response
        cout << "SERVER> " << string(buffer, bytesRcv) << "\r\n";

    } while (true);

    // Close socket
    close(sock);

    return 0;
}