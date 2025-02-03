#include "../inc/Server.hpp"

void    Server::addClient(int socket_fd, struct sockaddr_in addr) {
    if (this->_clientCount < MAX_CLIENTS) {
        std::cout << "new client connected\n";
        Client  newClient(socket_fd, addr);
        this->_clients[socket_fd] = newClient;
        struct pollfd   new_pollfd;
        new_pollfd.fd = socket_fd;
        new_pollfd.events = POLLIN;
        new_pollfd.revents = 0;
        this->_fds.push_back(new_pollfd);
    }
    else {
        close(socket_fd);
    }
}

void    Server::run() {

    struct sockaddr_in  clientAddr;
    socklen_t   clientLen = sizeof(clientAddr);
    while (true) {
        int pollResult = poll(this->_fds.data(), this->_fds.size(), -1);
        if (pollResult == -1) {
            std::cout << "poll failed\n";
        }
        for (size_t i = 0; i < this->_fds.size(); ++i) {
            if (this->_fds[i].revents & POLLIN) {
                if (this->_fds[i].fd == this->_socket_fd) {
                    int client_fd = accept(this->_socket_fd, (struct sockaddr*)&clientAddr, &clientLen);
                    if (client_fd != -1)
                        addClient(client_fd, clientAddr);
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
    this->_socket_fd = createSocket();
    struct pollfd   pollfd;
    pollfd.fd = this->_socket_fd;
    pollfd.events = POLLIN;
    pollfd.revents = 0;
    this->_fds.push_back(pollfd);
}

Server::~Server() {
    close(this->_socket_fd);
}

int Server::createSocket() {
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

Server::Server() {}

Server::Server(const Server& rhs) {
    (void)rhs;
}

Server& Server::operator=(const Server& rhs) {
    (void)rhs;
    return (*this);
}