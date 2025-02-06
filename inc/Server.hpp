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
#include <csignal>
#include <netdb.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 512

class Client;

extern volatile sig_atomic_t running;

class Server {
private:
    int         _listeningSocket;
    std::string _port;
    std::string _password;

    std::map<int, Client> _clients;

    int         _clientCount;

    Server();
    Server(const Server& rhs);
    Server& operator=(const Server& rhs);

    int     _createSocket();
    void    _addClient(std::vector<pollfd>& poll_fds);
    void    _removeClient(int fd, std::vector<pollfd>& poll_fds);
    
public:
    void    run();

    Server(std::string port, std::string password);
    ~Server();
};
