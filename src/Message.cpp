#include "../inc/Message.hpp"

Message::Message(const std::string& msg) {
    if (msg.empty())
        return ;

    std::istringstream  iss(msg);

    if (msg[0] == ':')
        iss >> this->_prefix;
    if (!(iss >> this->_command))
        return ;

    std::string token;
    while (iss >> token) {
        if (token[0] == ':') {
            std::string trailing;
            std::getline(iss, trailing);
            token = token.substr(1) + trailing;
            this->_params.push_back(token);
            break ;
        }
        this->_params.push_back(token);
    }
}

std::string Message::getPrefix() const { return (this->_prefix); }
std::string Message::getCommand() const { return (this->_command); }
std::vector<std::string> Message::getParams() const { return (this->_params); }
bool    Message::isValid() const { return (!this->_command.empty()); }

Message::Message() {}

Message::Message(const Message& rhs) {
    *this = rhs;
}

Message& Message::operator=(const Message& rhs) {
    if (this != &rhs) {
        this->_prefix = rhs._prefix;
        this->_command = rhs._command;
        this->_params = rhs._params;
    }
    return (*this);
}

Message::~Message() {}