#pragma once

#include "Numerics.hpp"
#include "Client.hpp"
#include <set>

using std::string;
using std::map;
using std::set;

class Client;

class Channel {
private:
    string              _name;
    map<int, Client*>   _clients;
    set<int>            _operators;     // o
    set<int>            _invitees;      // i
    bool                _inviteOnly;    // i
    bool                _topicLock;     // t
    int                 _userLimit;     // l
    int                 _userCount;     // l
    string              _key;           // k

public:
    bool    isInviteOnly() const;
    bool    isTopicLocked() const;
    bool    isClientInvited(Client& client) const;
    bool    isClientInChannel(const string& nickname) const;
    bool    isClientOp(Client& client) const;
    bool    hasChannelKey() const;
    bool    hasUserLimit() const;

    int     addClient(Client& client, const string& key);
    void    removeClient(Client& client);
    void    addInvitee(Client& client);
    void    removeInvitee(Client& client);
    void    addOperator(Client& client);
    void    removeOperator(Client& client);

    void    broadcastMessage(const string& message, const string& command, const string& sender);
    void    broadcastMessage(const string& message, const string& command, const Client& client);

    void    setInviteOnly(bool inviteOnly);
    void    setTopicLock(bool topicLock);
    void    setUserLimit(int userLimit);
    void    setKey(const string& key);

    const string&   getName() const;
    const string&   getKey() const;
    int             getUserLimit() const;
    Client*         getClient(const string& nickname);


    Channel();
    Channel(const string& channelName, Client& client);
    Channel(const Channel& rhs);
    Channel& operator=(const Channel& rhs);
    ~Channel();
};

typedef std::map<string, Channel>    channelmap_t;
