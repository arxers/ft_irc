#pragma once

#include "Client.hpp"

class Channel {
private:
    clientmap_t _clients;
public:
    void    addClient(int fd, Client client);
    // Channel(/* args */);
    // ~Channel();
};

typedef std::map<string, Channel>    channelmap_t;
