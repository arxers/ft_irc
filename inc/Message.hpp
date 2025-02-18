#pragma once

#include <string>
#include <vector>
#include <sstream>

class Message {
private:
    std::string                 _prefix;
    std::string                 _command;
    std::vector<std::string>    _params;

    Message();
public:
    std::string                 getPrefix() const;
    std::string                 getCommand() const;
    std::vector<std::string>    getParams() const;
    bool                        isValid() const;

    Message(const std::string&);
    Message(const Message&);
    Message& operator=(const Message&);
    ~Message();
};
