#pragma once

#include "Client.hpp"

class Channel {
private:
    clientmap_t _clients;
    clientmap_t _operators;
    bool        _inviteOnly;
    bool        _topicRestricted;
    int         _userLimit;
    string      _key;
public:
    void    addClient(int fd, Client client);
    void    setKey(const string& key);
    
    const string& getKey();

    // Channel(/* args */);
    // ~Channel();
};

typedef std::map<string, Channel>    channelmap_t;
