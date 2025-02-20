#pragma once

#include <string>
#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>

using std::string;
using std::vector;

enum    e_client_state {
    CONNECTED,
    AUTHENTICATED,
    REGISTERED,
    DISCONNETED
};

class Client {
private:
    int         _socket_fd;
    sockaddr_in _addr;
    string      _inputBuffer;
    string      _outputBuffer;

    e_client_state _state;
    bool    _op;
    string  _nickname;
    string  _username;
    string  _realname;
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
    const string    getIp() const;
    e_client_state  getState() const;
    const string&   getNickname() const;
    const string&   getUsername() const;
    const vector<string>& getChannels() const;

    void    setState(e_client_state state);
    void    setNickname(const string& nickname);
    void    setUsername(const string& Username);
    void    setRealname(const string& Realname);

    Client();
    Client(const Client& rhs);
    Client(int socket_fd, struct sockaddr_in addr);
    ~Client();

    Client& operator=(const Client& rhs);
};
