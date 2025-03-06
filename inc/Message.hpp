#pragma once

#include <string>
#include <vector>
#include <sstream>

using std::string;
using std::vector;

class Message {
private:
    string          _prefix;
    string          _command;
    vector<string>  _params;

    Message();
public:
    string          getPrefix() const;
    string          getCommand() const;
    vector<string>  getParams() const;
    // bool            isValid() const;

    Message(const string&);
    Message(const Message&);
    Message& operator=(const Message&);
    ~Message();
};
