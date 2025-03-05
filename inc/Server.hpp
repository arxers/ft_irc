#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <utility>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <csignal>
#include <cctype>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <netdb.h>
#include <fcntl.h>

#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"
#include "Numerics.hpp"

#ifndef HOSTNAME
    #define HOSTNAME    "0.0.0.0"
#endif
#define MAX_CLIENTS     100
#define MAX_MSG_LEN     512
#define PING_TIMEOUT    99999

using std::map;
using std::string;
using std::vector;
using std::pair;
using std::set;
using std::cout;
using std::cerr;


extern volatile sig_atomic_t running;

enum e_command{
    CAP,
    PASS,
    NICK,
    USER,
    JOIN,
    PART,
    PRIVMSG,
    KICK,
    INVITE,
    TOPIC,
    MODE,
    PING,
    PONG,
    QUIT,
};

typedef map<string, e_command>  commandmap_t;

class Client;
class Server {
private:
    int     _listeningSocket;
    string  _name;
    string  _port;
    string  _password;
    int     _clientCount;

    vector<pollfd>          _pollFds;
    set<int>                _disconnecting;
    set<string>             _emptyChannels;

    map<int, Client>        _clients;
    map<string, Channel>    _channels;
    map<string, e_command>  _commands;
    map< string, set<int> > _pendingInvites;

    Server(const Server&);
    Server& operator=(const Server&);

    int         _createSocket();
    void        _addClient();
    void        _removeClient(int fd);
    void        _handleClient(int clientFd);
    string      _generateResponse(Client& client, Message message);
    void        _flushClientBuffer(Client& client);
    Client*     _getClientByNickname(const string& nickname);
    Channel*    _getChannelByName(const string& channel);
    bool        _isValidNickname(const string& nickname);
    bool        _hasChannel(const string& channel);

    string  _sendWelcomeBurst(Client& client);

    // Commands
    string  _cap(const vector<string>& params);
    string  _pass(Client& client, const vector<string>& params);
    string  _nick(Client& client, const vector<string>& params);
    string  _user(Client& client, const vector<string>& params);
    string  _join(Client& client, const vector<string>& params);
    string  _part(Client& client, const vector<string>& params);
    string  _privMsg(Client& client, const vector<string>& params);
    string  _kick(Client& client, const vector<string>& params);
    string  _invite(Client& client, const vector<string>& params);
    string  _topic(Client& client, const vector<string>& params);
    string  _mode(Client& client, const vector<string>& params);
    string  _ping(Client& client, const vector<string>& params);
    string  _pong(Client& client, const vector<string>& params); 
    string  _quit(Client& client, const vector<string>& params);
    
public:
    void    start();
    void    init(string name, string port, string password);

    Server();
    ~Server();
};
