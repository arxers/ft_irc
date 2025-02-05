#pragma once

#include <string>
#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>

class Client {
private:
    int                 _socket_fd;
    struct sockaddr_in  _addr;

    bool                _op;
    bool                _authenticated;
    std::string         _nickname;
    std::string         _username;
    std::string         _password;

    std::vector<std::string> _channels;


public:
    void    authenticate();
    void    addChannel(const std::string& channel);
    void    removeChannel(const std::string& channel);

    // Predicates, getters, setters
    bool    isOperator() const ;
    bool    isAuthenticated() const;
    bool    isConnected() const;
    int     getSocket() const;
    const std::string   getIp() const;
    const std::string&  getNickname() const;
    const std::string&  getUsername() const;
    const std::vector<std::string>& getChannels() const;

    void    setNickname(const std::string& nickname);

    Client();
    Client(const Client& rhs);
    Client(int socket_fd, struct sockaddr_in addr);
    ~Client();

    Client& operator=(const Client& rhs);
};
