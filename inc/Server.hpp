#pragma once

#include <string>
#include <iostream>

class Server {
private:
    int         _port;
    std::string _password;

    Server();
    Server(const Server& rhs);
    Server& operator=(const Server& rhs);
public:
    ~Server();
};
