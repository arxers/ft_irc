#include "../inc/Server.hpp"

void    Server::run() {

}

Server::Server(int port, std::string password) :
_port(port), _password(password) {
    this->_socket_fd = createSocket();
}

Server::~Server() {
    close(this->_socket_fd);
}

int Server::createSocket() {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
        throw std::runtime_error("Failed to create socket");
    
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