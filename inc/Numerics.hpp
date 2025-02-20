#pragma once

#include <string>

using std::string;

#define CRLF "\r\n"

class Numerics {
private:
    Numerics();
    Numerics(const Numerics&);
    Numerics&    operator=(const Numerics&);
public:
    // Welcome Burst / MOTD
    static string   RPL_WELCOME(const string& server, const string& nick); //001
    static string   RPL_YOURHOST(const string& server, const string& nick, const string& servername); //002
    static string   RPL_CREATED(const string& server, const string& nick, const string& datetime); //003
    // static string   RPL_MYINFO() //004
    static string   RPL_MOTD(const string& server, const string& nick, const string& line); //372
    static string   RPL_MOTDSTART(const string& server, const string& nick, const string& info); //375
    static string   RPL_ENDOFMOTD(const string& server, const string& nick); //376
    static string   ERR_UNKNOWNCOMMAND(const string& server, const string& nick, const string& command); //421
    static string   ERR_NOMOTD(const string& server, const string& nick); //422

    // Operator
    static string   RPL_YOUREOPER(const string& server, const string& nick); //381
    static string   ERR_NEEDMOREPARAMS(const string& server, const string& nick, const string& command); //461
    static string   ERR_NOPRIVILEGES(const string& server, const string& nick); //481
    // static string   ERR_NOOPERHOST(); //491
    static string   ERR_NOPRIVS(const string& server, const string& nick, const string& cmd); //723

    static string   formatMessage(const string& server, const string& num, const string& nick, const string& message) {
        return (":" + server + " " + num + " " + nick + " :" + message + CRLF);
    }

    static string   formatMessage(const string& server, const string& num, const string& nick, const string& command, const string& message) {
        return (":" + server + " " + num + " " + nick + " " + command + " :" + message  + CRLF);
    }
};
