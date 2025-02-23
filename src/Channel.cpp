#include "Channel.hpp"

void    Channel::addClient(int fd, Client client) {
    this->_clients[fd] = client;
}
    // Channel(/* args */);
    // ~Channel();

