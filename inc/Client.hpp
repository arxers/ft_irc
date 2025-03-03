#pragma once

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctime>

#include "Channel.hpp"

using std::string;
using std::vector;
using std::map;

enum    e_client_state {
    CONNECTED,
    AUTHENTICATED,
    REGISTERED,
};

class Channel;

class Client {
private:
    int         _socketFd;
    sockaddr_in _addr;
    string      _inputBuffer;
    string      _outputBuffer;

    e_client_state _state;
    bool    _op;
    string  _nickname;
    string  _username;
    string  _realname;

    time_t  _lastActiveTime;
    time_t  _lastPingTime;
    bool    _pinged;

    vector<string>    _channels;


public:
    void    authenticate();
    void    addChannel(const string& channel);
    void    removeChannel(const string& channel);
    void    sendMessage(const string& message, const string& sender);

    // Predicates, getters, setters
    bool    isOperator() const ;
    bool    isAuthenticated() const;
    bool    isConnected() const;
    bool    isInChannel(const string& channel);
    bool    isPinged();

    string&    getInputBuffer();
    string&    getOutputBuffer();
    
    int             getSocket() const;
    const string    getIp() const;
    e_client_state  getState() const;
    const string&   getNickname() const;
    const string&   getUsername() const;
    const vector<string>& getChannels() const;
    time_t  getIdleTime() const;
    time_t  getTimeSinceLastPing() const;

    void    setState(e_client_state state);
    void    setNickname(const string& nickname);
    void    setUsername(const string& Username);
    void    setRealname(const string& Realname);
    void    setLastActiveTime();
    void    setLastPingTime();
    void    setPinged(bool pinged);

    Client();
    Client(const Client& rhs);
    Client(int socketFd, struct sockaddr_in addr);
    ~Client();

    Client& operator=(const Client& rhs);
};

typedef std::map<int, Client> clientmap_t;