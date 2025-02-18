#pragma once

#include <string>
#include <vector>
#include <sstream>

class Message {
private:
    std::string                 prefix;
    std::string                 command;
    std::vector<std::string>    params;

    Message();
    Message(const Message&);
    Message& operator=(const Message&);
public:
    std::string                 getPrefix() const;
    std::string                 getCommand() const;
    std::vector<std::string>    getParams() const;

    Message(const std::string&);
    ~Message();
};
