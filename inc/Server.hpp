#pragma once

#include <string>
#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <map>
#include "Client.hpp"

#define MAX_CLIENTS 100

class Server {
private:
    int         _socket_fd;
    int         _port;
    std::string _password;

    std::map<int, Client> _clients;

    int createSocket();

    Server();
    Server(const Server& rhs);
    Server& operator=(const Server& rhs);
public:
    void    run();

    Server(int port, std::string password);
    ~Server();
};
