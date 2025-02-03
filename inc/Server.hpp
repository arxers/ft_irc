#pragma once

#include <string>
#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <map>
#include <poll.h>
#include <vector>
#include "Client.hpp"

#define MAX_CLIENTS 100

class Client;

class Server {
private:
    int         _socket_fd;
    int         _port;
    std::string _password;

    std::map<int, Client> _clients;
    std::vector<struct pollfd>  _fds;

    int         _clientCount;

    int createSocket();

    Server();
    Server(const Server& rhs);
    Server& operator=(const Server& rhs);
public:
    void    run();
    void    addClient(int socket_fd, struct sockaddr_in addr);

    Server(int port, std::string password);
    ~Server();
};
