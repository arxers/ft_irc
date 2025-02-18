#include "../inc/Message.hpp"

Message::Message(const std::string& msg) {
    std::istringstream  iss(msg);

    if (msg[0] == ':')
        iss >> this->prefix;
    if (!(iss >> this->command))
        return ;

    std::string token;
    while (iss >> token) {
        if (token[0] == ':') {
            std::string trailing;
            std::getline(iss, trailing);
            token = token.substr(1) + trailing;
            this->params.push_back(token);
            break ;
        }
        this->params.push_back(token);
    }
}

std::string Message::getPrefix() const { return (this->prefix); }
std::string Message::getCommand() const { return (this->command); }
std::vector<std::string> Message::getParams() const { return (this->params); }

Message::Message() {}
Message::Message(const Message&) {}
Message& Message::operator=(const Message&) { return (*this); }
Message::~Message() {}