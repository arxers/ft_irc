#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <csignal>
#include <cctype>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <netdb.h>

#include "Client.hpp"
#include "Message.hpp"
#include "Numerics.hpp"

#define MAX_CLIENTS 100
#define MAX_MSG_LEN 512

using std::map;
using std::string;
using std::vector;


extern volatile sig_atomic_t running;

enum e_command{
    PASS,
    NICK,
    USER,
    JOIN,
    PRIVMSG,
    KICK,
    INVITE,
    TOPIC,
    MODE,
};

typedef map<string, e_command> command_map;
typedef map<int, Client> client_map;

class Client;
class Server {
private:
    int     _listeningSocket;
    string  _name;
    string  _port;
    string  _password;

    int         _clientCount;
    client_map  _clients;
    command_map _commands;

    Server();
    Server(const Server&);
    Server& operator=(const Server&);

    int     _createSocket();
    void    _addClient(vector<pollfd>& poll_fds);
    void    _removeClient(int fd, vector<pollfd>& poll_fds);
    void    _handleClient(vector<pollfd>& poll_fds, struct pollfd& client_pfd);
    string  _generateResponse(int client_fd, string& input);

    // Commands
    string pass(int client_fd, const vector<string>& params);
    
public:
    void    run();

    Server(string name, string port, string password);
    ~Server();
};
