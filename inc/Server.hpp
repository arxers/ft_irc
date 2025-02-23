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
#include "Channel.hpp"
#include "Message.hpp"
#include "Numerics.hpp"

#define MAX_CLIENTS 100
#define MAX_MSG_LEN 512

using std::map;
using std::string;
using std::vector;
using std::cout;
using std::cerr;


extern volatile sig_atomic_t running;

enum e_command{
    CAP,
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

typedef std::map<string, e_command>  commandmap_t;

class Client;
class Server {
private:
    int     _listeningSocket;
    string  _name;
    string  _port;
    string  _password;

    int             _clientCount;
    clientmap_t     _clients;
    channelmap_t    _channels;
    commandmap_t    _commands;

    Server();
    Server(const Server&);
    Server& operator=(const Server&);

    int     _createSocket();
    void    _addClient(vector<pollfd>& poll_fds);
    void    _removeClient(int fd, vector<pollfd>& poll_fds);
    void    _handleClient(vector<pollfd>& poll_fds, struct pollfd& client_pfd);
    string  _generateResponse(Client& client, Message message);
    void    _sendToClient(Client& client);
    Client* _getClientByNickname(const string& nickname);
    bool    _isValidNickname(const string& nickname);

    // Commands
    string  cap(const vector<string>& params);
    string  pass(Client& client, const vector<string>& params);
    string  nick(Client& client, const vector<string>& params);
    string  user(Client& client, const vector<string>& params);
    string  join(Client& client, const vector<string>& params);
    
public:
    void    run();

    Server(string name, string port, string password);
    ~Server();
};
