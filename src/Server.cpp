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
            std::cerr << "setsockopt() failed\n";
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

void    Server::_addClient(std::vector<struct pollfd>& poll_fds) {
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
        std::cout << inet_ntoa(clientAddr.sin_addr) <<  " connected\n";

        string msg(inet_ntoa(clientAddr.sin_addr));
            msg += " connected\n";
        for (map<int, Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it) {
            if (it->first != client_fd) {
                send(it->first, msg.c_str(), msg.size(), 0);
            }
        }
        return ;
    }
    close(client_fd);
}

void    Server::_removeClient(int socket_fd, std::vector<pollfd>& poll_fds) {
    std::cout << this->_clients[socket_fd].getIp() << " disconnected\n";
    this->_clients.erase(socket_fd);
    for (std::vector<pollfd>::iterator it = poll_fds.begin(); it != poll_fds.end(); ++it) {
        if (it->fd == socket_fd) {
            poll_fds.erase(it);
            this->_clientCount--;
            close(socket_fd);
            return ;
        }
    }
}

void    Server::_handleClient(std::vector<pollfd>& poll_fds, struct pollfd& client_pfd) {
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
        outputBuffer += _generateResponse(client_pfd.fd, input);
        inputBuffer.erase(0, pos + 2);
    }
}

static string  strToUpper(string s) {
    for (string::iterator it = s.begin(); it != s.end(); ++it)
        *it = (std::toupper(*it));
    return (s);
}

string  Server::pass(int client_fd, const vector<string>& params) {
    Client client = this->_clients[client_fd];
    if (params.size() < 1)
        return (Numerics::ERR_NEEDMOREPARAMS(this->_name, client.getNickname(), "PASS"));
    if (params[0] == this->_password) {
        client.authenticate();
        return ("");
    }
    return ("THATS THE WRONG NUMBAH\r\n");
}

string Server::_generateResponse(int client_fd, string& input) {
    Message message(input);

    string commandUpper = strToUpper(message.getCommand());
    command_map::iterator   it = this->_commands.find(commandUpper);

    std::string reply;
    e_command command = it->second;
    switch (command) {
        case PASS:
            std::cout << "Token found: PASS\n";
            reply = pass(client_fd, message.getParams());
            break ;
        case NICK:
            std::cout << "Token found: NICK\n";     break ;
        case USER:
            std::cout << "Token found: USER\n";     break ;
        case JOIN:
            std::cout << "Token found: JOIN\n";     break ;
        case PRIVMSG:
            std::cout << "Token found: PRIVMSG\n";  break ;
        case KICK:
            std::cout << "Token found: KICK\n";     break ;
        case INVITE:
            std::cout << "Token found: INVITE\n";   break ;
        case TOPIC:
            std::cout << "Token found: TOPIC\n";    break ;
        case MODE:
            std::cout << "Token found: MODE\n";     break ;
        default:
            std::cout << "Token not found\n";
    }
    std::cout << reply;
    return (reply);
}

void    Server::run() {
    std::vector<struct pollfd>  poll_fds;

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
    }
}

static bool isValidPort(int n) {
    return (n >= 1024 && n <= 49151);
}

static void initCommandMap(command_map& map) {
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