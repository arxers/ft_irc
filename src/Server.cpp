#include "../inc/Server.hpp"

int Server::_createSocket() {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
        throw std::runtime_error("Failed to open socket");
    
    int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        throw std::runtime_error("setsockopt() failed");

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(this->_port);

    if (bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
        throw std::runtime_error("bind() failed");

    if (listen(socket_fd, MAX_CLIENTS) == -1)
        throw std::runtime_error("listen() failed");

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
        std::cout << "new client connected\n";
        this->_clients[client_fd] = Client(client_fd, clientAddr);
        struct pollfd   client_pfd = {client_fd, POLLIN, 0};
        poll_fds.push_back(client_pfd);
    }
    else
        close(client_fd);
}

void    Server::_removeClient(int socket_fd, std::vector<pollfd>& poll_fds) {
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

void    Server::run() {
    std::vector<struct pollfd>  poll_fds;

    struct pollfd   server_pfd = {this->_listeningSocket, POLLIN, 0};
    poll_fds.push_back(server_pfd);

    while (true) {
        int pollResult = poll(poll_fds.data(), poll_fds.size(), -1);
        if (pollResult == -1) {
            std::cout << "poll failed\n";
        }
        for (size_t i = 0; i < poll_fds.size(); ++i) {
            if (poll_fds[i].revents & POLLIN) {
                if (poll_fds[i].fd == this->_listeningSocket) {
                    _addClient(poll_fds);
                }
                else {
                    ; // client
                }
            }
        }
    }
}

Server::Server(int port, std::string password) :
_port(port), _password(password) {
    this->_listeningSocket = _createSocket();

}

Server::~Server() {
    close(this->_listeningSocket);

    for (std::map<int, Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it) {
        close(it->first);
    }
}

Server::Server() {}

Server::Server(const Server& rhs) {
    (void)rhs;
}

Server& Server::operator=(const Server& rhs) {
    (void)rhs;
    return (*this);
}