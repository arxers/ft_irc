#pragma once

#include <string>
#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>

using std::string;
using std::vector;

class Client {
private:
    int         _socket_fd;
    sockaddr_in _addr;
    string      _inputBuffer;
    string      _outputBuffer;

    bool    _op;
    bool    _authenticated;
    string  _nickname;
    string  _username;
    string  _password;

    vector<string> _channels;


public:
    void    authenticate();
    void    addChannel(const string& channel);
    void    removeChannel(const string& channel);

    // Predicates, getters, setters
    bool    isOperator() const ;
    bool    isAuthenticated() const;
    bool    isConnected() const;
    int     getSocket() const;
    string&    getInputBuffer();
    string&    getOutputBuffer();
    const string   getIp() const;
    const string&  getNickname() const;
    const string&  getUsername() const;
    const vector<string>& getChannels() const;

    void    setNickname(const string& nickname);

    Client();
    Client(const Client& rhs);
    Client(int socket_fd, struct sockaddr_in addr);
    ~Client();

    Client& operator=(const Client& rhs);
};
