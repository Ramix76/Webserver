#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring> // For memset
#include <cerrno> // For errno
#include <fstream>
#include <sstream>
#include <map>

class HTTPRequest
{
    private:
        std::string method;
        std::string path;
        std::map<std::string, std::string> headers;
        std::string body;

    public:
        HTTPRequest(const std::string& request);

        std::string getMethod() const { return method; }
        std::string getPath() const { return path; }
        std::string getHeader(const std::string& key) const;
        std::string getBody() const { return body; }
};

#endif /* HTTPREQUEST_HPP */