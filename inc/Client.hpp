#pragma once

#include <string>
#include <vector>
#include <netinet/in.h>

class Client {
private:
    int                 _socket_fd;
    struct sockaddr_in  _addr;

    bool                _authenticated;
    std::string         _nickname;
    const std::string   _username;

    std::vector<std::string> _channels;

    Client();
    Client(const Client& rhs);
    Client& operator=(const Client& rhs);
public:
    void    authenticate(const std::string& password);
    void    addChannel(const std::string& channel);
    void    removeChannel(const std::string& channel);

    // Predicates, getters, setters
    bool    isAuthenticated() const;
    bool    isConnected() const;
    int     getSocket() const;
    const std::string&  getNickname() const;
    const std::string&  getUsername() const;
    const std::vector<std::string>& getChannels() const;

    void    setNickname(const std::string& nickname);

    Client(int socket_fd);
    ~Client();
};
