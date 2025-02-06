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

        std::string msg(inet_ntoa(clientAddr.sin_addr));
            msg += " connected\n";
        for (std::map<int, Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it) {
            if (it->first != client_fd) {
                send(it->first, msg.c_str(), msg.size(), 0);
            }
        }
    }
    else
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

void    Server::run() {
    std::vector<struct pollfd>  poll_fds;

    running = true;
    struct pollfd   server_pfd = {this->_listeningSocket, POLLIN, 0};
    poll_fds.push_back(server_pfd);

    char    buf[BUFFER_SIZE];
    while (running) {
        int pollResult = poll(poll_fds.data(), poll_fds.size(), -1);
        if (pollResult == -1) {
            break ;
        }
        for (size_t i = 0; i < poll_fds.size(); ++i) {
            if (poll_fds[i].revents & POLLIN) {
                if (poll_fds[i].fd == this->_listeningSocket)
                    _addClient(poll_fds);
                else {
                    int bytesRead = recv(poll_fds[i].fd, buf, BUFFER_SIZE - 1, 0);
                    if (bytesRead == 0) {
                        _removeClient(poll_fds[i].fd, poll_fds);
                        continue ;
                    }
                    buf[bytesRead] = '\0';
                    std::cout << this->_clients[poll_fds[i].fd].getIp() << ": " << buf;
                    std::string msg(this->_clients[poll_fds[i].fd].getIp());
                    msg += ": " + std::string(buf);
                    for (std::map<int, Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it) {
                        if (it->first != poll_fds[i].fd) {
                            send(it->first, msg.c_str(), msg.size(), 0);
                        }
                    }
                }
            }
        }
    }
}

Server::Server(std::string port, std::string password) :
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