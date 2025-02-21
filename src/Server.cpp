#include "../inc/Server.hpp"

int Server::_createSocket() {
    struct addrinfo hints = {}, *res, *p;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo(NULL, this->_port.c_str(), &hints, &res) == -1)
        throw std::runtime_error("Failed to get address info");

    int socket_fd = -1;
    for (p = res; p != NULL; p = p->ai_next) {
        socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (socket_fd == -1)
            continue ;

        int opt = 1;
        if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
            cerr << "setsockopt() failed\n";
            continue ;
        }

        if (bind(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_fd);
            continue ;
        }
        break ;
    }
    if (socket_fd == -1) {
        freeaddrinfo(res);
        throw std::runtime_error("Failed to bind socket");
    }

    if (listen(socket_fd, MAX_CLIENTS) == -1) {
        close(socket_fd);
        freeaddrinfo(res);
        throw std::runtime_error("listen() failed");
    }

    freeaddrinfo(res);
    return (socket_fd);
}

void    Server::_addClient(vector<struct pollfd>& poll_fds) {
    sockaddr_in clientAddr;
    socklen_t   clientLen = sizeof(clientAddr);

    int client_fd = accept(this->_listeningSocket, (struct sockaddr*)&clientAddr, &clientLen);
    if (client_fd == -1)
        return ;

    if (this->_clientCount < MAX_CLIENTS) {
        this->_clientCount++;
        this->_clients[client_fd] = Client(client_fd, clientAddr);
        struct pollfd   client_pfd = {client_fd, POLLIN, 0};
        poll_fds.push_back(client_pfd);
        cout << inet_ntoa(clientAddr.sin_addr) <<  " connected\n";
        return ;
    }
    close(client_fd);
}

void    Server::_removeClient(int socket_fd, vector<pollfd>& poll_fds) {
    cout << this->_clients[socket_fd].getIp() << " disconnected\n";
    this->_clients.erase(socket_fd);
    for (vector<pollfd>::iterator it = poll_fds.begin(); it != poll_fds.end(); ++it) {
        if (it->fd == socket_fd) {
            poll_fds.erase(it);
            this->_clientCount--;
            close(socket_fd);
            return ;
        }
    }
}

void    Server::_handleClient(vector<pollfd>& poll_fds, struct pollfd& client_pfd) {
    char        buf[MAX_MSG_LEN + 1];
    int         receivedBytes;

    receivedBytes = recv(client_pfd.fd, buf, sizeof(buf), 0);
    if (receivedBytes == 0) {
        _removeClient(client_pfd.fd, poll_fds);
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
        cout << input;
        inputBuffer.erase(0, pos + 2);
    }
}

static string  strToUpper(string s) {
    for (string::iterator it = s.begin(); it != s.end(); ++it)
        *it = (std::toupper(*it));
    return (s);
}

string Server::cap(const vector<string>& params) {
    if (params.empty() || params[0] != "LS")
        return ("");
    return (Numerics::formatMessage(this->_name, "CAP * LS :"));
}

string  Server::pass(Client& client, const vector<string>& params) {
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

string  Server::nick(Client& client, const vector<string>& params) {
    if (params.size() < 1)
        return (Numerics::formatMessage(this->_name, "431", client.getNickname(), "No nickname given"));
    if (!_isValidNickname(params[0]))
        return (Numerics::formatMessage(this->_name, "432", client.getNickname(), params[0], "Erroneous nickname"));
    if (_getClientByNickname(params[0]))
        return (Numerics::formatMessage(this->_name, "433", client.getNickname(), params[0], "Nickname is already in use"));
    client.setNickname(params[0]);
    if (client.getUsername() != "")
        client.setState(REGISTERED);
    return ("");
}

string  Server::user(Client& client, const vector<string>& params) {
    if (params.size() < 4)
        return (Numerics::formatMessage(this->_name, "461", client.getNickname(), "Not enough parameters"));
    client.setUsername(params[0]);
    client.setRealname(params[3]);
    if (client.getNickname() != "*")
        client.setState(REGISTERED);
    return ("");
}

string  Server::join(Client& client, const vector<string>& params) {
    if (params.size() < 1)
        return (Numerics::formatMessage(this->_name, "461", client.getNickname(), "Not enough parameters"));
    return ("");
}

string Server::_generateResponse(Client& client, Message message) {
    string commandUpper = strToUpper(message.getCommand());
    command_map::iterator   it = this->_commands.find(commandUpper);

    string reply;
    e_command command = it->second;
    const vector<string>&   params = message.getParams();
    if (command == CAP)
        return (cap(params));

    if (client.getState() < AUTHENTICATED) {
        if (command != PASS)
            return (Numerics::formatMessage(this->_name, "451", client.getNickname(), "You have not registered"));
    }
    else if (client.getState() < REGISTERED) {
        if (command != NICK && command != USER)
            return (Numerics::formatMessage(this->_name, "451", client.getNickname(), "You have not registered"));
    }

    switch (command) {
        case PASS:      return (pass(client, params));
        case NICK:      return (nick(client, params));
        case USER:      return (user(client, params));
        case JOIN:      return (join(client, params));
        case PRIVMSG:   return ("PRIVMSG");
        case KICK:      return ("KICK");
        case INVITE:    return ("INVITE");
        case TOPIC:     return ("TOPIC");
        case MODE:      return ("MODE");
        default: return (Numerics::formatMessage(this->_name, "421", client.getNickname(), message.getCommand(), "Unknown command!"));
    }
}

void    Server::_sendToClient(Client& client) {
    int fd = client.getSocket();
    string& buf = client.getOutputBuffer();
    size_t  size = buf.size();

    if (!size)
        return ;
    // cout << buf;
    send(fd, buf.c_str(), size, 0);
    buf.clear();
}

void    Server::run() {
    vector<struct pollfd>  poll_fds;

    running = true;
    struct pollfd   server_pfd = {this->_listeningSocket, POLLIN, 0};
    poll_fds.push_back(server_pfd);

    while (running) {
        int pollResult = poll(poll_fds.data(), poll_fds.size(), -1);
        if (pollResult == -1) {
            break ;
        }
        for (size_t i = 0; i < poll_fds.size(); ++i) {
            if (poll_fds[i].revents & POLLIN) {
                if (poll_fds[i].fd == this->_listeningSocket)
                    _addClient(poll_fds);
                else
                    _handleClient(poll_fds, poll_fds[i]);
            }
        }
        for (size_t i = 0; i < poll_fds.size(); ++i) {
            if (this->_clients.find(poll_fds[i].fd) != this->_clients.end()) {
                Client& client = this->_clients[poll_fds[i].fd];
                _sendToClient(client);
            }
        }
    }
}

static bool isValidPort(int n) {
    return (n >= 1024 && n <= 49151);
}

static void initCommandMap(command_map& map) {
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