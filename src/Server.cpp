#include "../inc/Server.hpp"

int Server::_createSocket() {
    struct addrinfo hints = {}, *res, *p;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo(NULL, this->_port.c_str(), &hints, &res) == -1)
        throw std::runtime_error("Failed to get address info");

    int socketFd = -1;
    for (p = res; p != NULL; p = p->ai_next) {
        socketFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (socketFd == -1)
            continue ;

        int opt = 1;
        if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
            cerr << "setsockopt() failed\n";
            continue ;
        }

        if (bind(socketFd, p->ai_addr, p->ai_addrlen) == -1) {
            close(socketFd);
            continue ;
        }
        break ;
    }
    if (socketFd == -1) {
        freeaddrinfo(res);
        throw std::runtime_error("Failed to bind socket");
    }

    if (listen(socketFd, MAX_CLIENTS) == -1) {
        close(socketFd);
        freeaddrinfo(res);
        throw std::runtime_error("listen() failed");
    }

    freeaddrinfo(res);
    return (socketFd);
}

void    Server::_addClient(vector<struct pollfd>& pollFds) {
    sockaddr_in clientAddr;
    socklen_t   clientLen = sizeof(clientAddr);

    int clientFd = accept(this->_listeningSocket, (struct sockaddr*)&clientAddr, &clientLen);
    if (clientFd == -1)
        return ;

    if (this->_clientCount < MAX_CLIENTS) {
        this->_clientCount++;
        this->_clients[clientFd] = Client(clientFd, clientAddr);
        struct pollfd   client_pfd = {clientFd, POLLIN, 0};
        pollFds.push_back(client_pfd);
        cout << inet_ntoa(clientAddr.sin_addr) <<  " connected\n";
        return ;
    }
    close(clientFd);
}

void    Server::_removeClient(int socketFd, vector<pollfd>& pollFds) {
    cout << this->_clients[socketFd].getIp() << " disconnected\n";
    this->_clients.erase(socketFd);
    for (vector<pollfd>::iterator it = pollFds.begin(); it != pollFds.end(); ++it) {
        if (it->fd == socketFd) {
            pollFds.erase(it);
            this->_clientCount--;
            close(socketFd);
            return ;
        }
    }
}

void    Server::_handleClient(vector<pollfd>& pollFds, struct pollfd& client_pfd) {
    char        buf[MAX_MSG_LEN + 1];
    int         receivedBytes;

    receivedBytes = recv(client_pfd.fd, buf, sizeof(buf), 0);
    if (receivedBytes == 0) {
        _removeClient(client_pfd.fd, pollFds);
        return ;
    }

    string &inputBuffer = this->_clients[client_pfd.fd].getInputBuffer();
    inputBuffer.append(buf, receivedBytes);
    if (inputBuffer.length() > MAX_MSG_LEN) {
        inputBuffer = inputBuffer.substr(0, MAX_MSG_LEN - 2);
        inputBuffer += "\r\n";
    }
    
    string &outputBuffer = this->_clients[client_pfd.fd].getOutputBuffer();
    while (inputBuffer.find("\r\n") != string::npos) {
        size_t  pos = inputBuffer.find("\r\n");
        string input = inputBuffer.substr(0, pos + 2);
        outputBuffer += _generateResponse(this->_clients[client_pfd.fd], input);
        cout << "<" << client_pfd.fd << ": " << input;
        inputBuffer.erase(0, pos + 2);
    }
}

static string  strToUpper(string s) {
    for (string::iterator it = s.begin(); it != s.end(); ++it)
        *it = (std::toupper(*it));
    return (s);
}

string Server::_cap(const vector<string>& params) {
    if (params.empty() || params[0] != "LS")
        return ("");
    return (Numerics::formatMessage(this->_name, "CAP * LS :"));
}

string  Server::_pass(Client& client, const vector<string>& params) {
    if (client.isAuthenticated())
        return (Numerics::formatMessage(this->_name, "462", client.getNickname(), "Unauthorized command (already registered)"));
    if (params.size() < 1)
        return (Numerics::formatMessage(this->_name, "461", client.getNickname(), "Not enough parameters"));
    if (params[0] == this->_password) {
        client.authenticate();
        return ("");
    }
    return (Numerics::formatMessage(this->_name, "464", client.getNickname(), "Password incorrect"));
}

Client* Server::_getClientByNickname(const string& nickname) {
    for (size_t i = 0; i < this->_clients.size(); ++i) {
        if (this->_clients[i].getNickname() == nickname)
            return (&this->_clients[i]);
    }
    return (NULL);
}

bool    Server::_isValidNickname(const string& nickname) {
    if (nickname.length() > 9)
        return (false);

    string letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    string digit = "1234567890";
    string special = "[]\\_^{}`";

    if (letters.find(nickname[0]) == string::npos &&
        special.find(nickname[0]) == string::npos)
        return (false);

    for (size_t i = 1; i < nickname.length(); ++i) {
        if (letters.find(nickname[i]) == string::npos &&
            special.find(nickname[i]) == string::npos &&
            digit.find(nickname[i]) == string::npos &&
            nickname[i] != '-')
            return (false);
    }

    return (true);
}

string  Server::_nick(Client& client, const vector<string>& params) {
    if (params.size() < 1)
        return (Numerics::formatMessage(this->_name, "431", client.getNickname(), "No nickname given"));
    if (!_isValidNickname(params[0]))
        return (Numerics::formatMessage(this->_name, "432", client.getNickname(), params[0], "Erroneous nickname"));
    if (_getClientByNickname(params[0]))
        return (Numerics::formatMessage(this->_name, "433", client.getNickname(), params[0], "Nickname is already in use"));
    client.setNickname(params[0]);
    if (client.getState() == AUTHENTICATED && client.getUsername() != "") {
        client.setState(REGISTERED);
        return (Numerics::formatMessage(this->_name, "001", client.getNickname(), "Welcome to the Internet Relay Network, " + client.getNickname()));
    }
    return ("");
}

string  Server::_user(Client& client, const vector<string>& params) {
    if (params.size() < 4)
        return (Numerics::formatMessage(this->_name, "461", client.getNickname(), "Not enough parameters"));
    client.setUsername(params[0]);
    client.setRealname(params[3]);
    if (client.getState() == AUTHENTICATED && client.getNickname() != "*") {
        client.setState(REGISTERED);
        return (Numerics::formatMessage(this->_name, "001", client.getNickname(), "Welcome to " + this->_name + ", " + client.getNickname()));
    }
    return ("");
}

bool    isValidChannelName(const std::string& channel) {
    if (channel.empty())
        return (false);
    if (channel[0] != '#')
        return (false);
    static const string invalidChars(" ,:");
    for (size_t i = 1; i < channel.length(); ++i)
        if (!std::isprint(channel[i]) || invalidChars.find(channel[i]) != string::npos)
            return (false);
    return (true);
}

string  Server::_join(Client& client, const vector<string>& params) {
    if (params.size() < 1)
        return (Numerics::formatMessage(this->_name, "461", client.getNickname(), "Not enough parameters"));
    // check channel name
    if (!isValidChannelName(params[0]))
        return Numerics::formatMessage(this->_name, "403", client.getNickname(), params[0], "No such channel");
    
    channelmap_t::iterator it = this->_channels.find(params[0]);
    if (it != this->_channels.end()) {
        it->second.addClient(client);
        return ("");
    }

    Channel newChannel(params[0], client);
    this->_channels[params[0]] = newChannel;
    client.addChannel(newChannel);
    return ("");
}

string  Server::_privMsg(Client& client, const vector<string>& params) {
    if (params[0][0] == '#') {
        channelmap_t::iterator it = this->_channels.find(params[0]);
        if (it == this->_channels.end())
            return (Numerics::formatMessage(this->_name, "401", client.getNickname(), "No such nick/channel"));
        it->second.broadcastMessage(params[1], client);
    }
            
    return ("");
}

string  Server::_quit(Client& client, const vector<string>& params) {
    (void)client;
    (void)params;
    return ("");
}

string Server::_generateResponse(Client& client, Message message) {
    string commandUpper = strToUpper(message.getCommand());
    commandmap_t::iterator   it = this->_commands.find(commandUpper);

    string reply;
    e_command command = it->second;
    const vector<string>&   params = message.getParams();
    if (command == CAP)
        return (_cap(params));

    if (client.getState() < AUTHENTICATED) {
        if (command != PASS)
            return (Numerics::formatMessage(this->_name, "451", client.getNickname(), "You have not registered"));
    }
    else if (client.getState() < REGISTERED) {
        if (command != NICK && command != USER)
            return (Numerics::formatMessage(this->_name, "451", client.getNickname(), "You have not registered"));
    }

    switch (command) {
        case PASS:      return (_pass(client, params));
        case NICK:      return (_nick(client, params));
        case USER:      return (_user(client, params));
        case JOIN:      return (_join(client, params));
        case PRIVMSG:   return (_privMsg(client, params));
        case KICK:      return ("KICK\n");
        case INVITE:    return ("INVITE\n");
        case TOPIC:     return ("TOPIC\n");
        case MODE:      return ("MODE\n");
        case QUIT:      return (_quit(client, params));
        default: return (Numerics::formatMessage(this->_name, "421", client.getNickname(), message.getCommand(), "Unknown command!"));
    }
}

void    Server::_sendToClient(Client& client) {
    int fd = client.getSocket();
    string& buf = client.getOutputBuffer();
    size_t  size = buf.size();

    if (!size)
        return ;
    cout << ">" << client.getSocket() << buf;
    send(fd, buf.c_str(), size, 0);
    buf.clear();
}

void    Server::run() {
    vector<struct pollfd>  pollFds;

    running = true;
    struct pollfd   server_pfd = {this->_listeningSocket, POLLIN, 0};
    pollFds.push_back(server_pfd);

    while (running) {
        int pollResult = poll(pollFds.data(), pollFds.size(), -1);
        if (pollResult == -1) {
            break ;
        }
        for (size_t i = 0; i < pollFds.size(); ++i) {
            if (pollFds[i].revents & POLLIN) {
                if (pollFds[i].fd == this->_listeningSocket)
                    _addClient(pollFds);
                else
                    _handleClient(pollFds, pollFds[i]);
            }
        }
        for (size_t i = 0; i < pollFds.size(); ++i) {
            if (this->_clients.find(pollFds[i].fd) != this->_clients.end()) {
                Client& client = this->_clients[pollFds[i].fd];
                _sendToClient(client);
            }
        }
    }
}

static bool isValidPort(int n) {
    return (n >= 1024 && n <= 49151);
}

static void initCommandMap(commandmap_t& map) {
    map["CAP"]      = CAP;
    map["PASS"]     = PASS;
    map["NICK"]     = NICK;
    map["USER"]     = USER;
    map["JOIN"]     = JOIN;
    map["PRIVMSG"]  = PRIVMSG;
    map["KICK"]     = KICK;
    map["INVITE"]   = INVITE;
    map["TOPIC"]    = TOPIC;
    map["MODE"]     = MODE;
    map["QUIT"]     = QUIT;
}

Server::Server(string name, string port, string password) :
_name(name), _port(port), _password(password), _clientCount(0) {
    if (!isValidPort(std::atoi(this->_port.c_str())))
        throw std::invalid_argument("Port must be within 1024-49151");
    this->_listeningSocket = _createSocket();
    initCommandMap(this->_commands);
}

Server::~Server() {
    close(this->_listeningSocket);

    for (map<int, Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it) {
        close(it->first);
    }
}

Server::Server() {}
Server::Server(const Server&) {}
Server& Server::operator=(const Server&) { return (*this); }