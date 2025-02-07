#pragma once

#include <string>

using std::string;

class Numerics {
private:
    Numerics();
    Numerics(const Numerics&);
    Numerics&    operator=(const Numerics&);
public:
    // Welcome Burst / MOTD
    static string   RPL_WELCOME(const string& server, const string& nick); //001
    static string   RPL_YOURHOST(const string& server, const string& nick, const string& servername); //002
    static string   RPL_CREATED(const string& server, const string& nick); //003
    // static string   RPL_MYINFO() //004
    static string   RPL_MOTD(const string& server, const string& nick); //372
    static string   RPL_MOTDSTART(const string& server, const string& nick, const string& info); //375
    static string   RPL_ENDOFMOTD(const string& server, const string& nick); //376
    static string   ERR_NOMOTD(const string& server, const string& nick); //422

    // Operator
    static string   RPL_YOUREOPER(const string& server, const string& nick); //381
    static string   ERR_NOPRIVILEGES(const string& server, const string& nick); //481
    // static string   ERR_NOOPERHOST(); //491
    static string   ERR_NOPRIVS(const string& server, const string& nick); //723
};

static string  Numerics::RPL_WELCOME(const string& server, const string& nick) {
    return (":" + server + " 001 " + nick + " :Welcome to the IRC Network, " + nick + '!'); }

static string  Numerics::RPL_YOURHOST(const string& server, const string& nick, const string& servername) {
    return (":" + server + " 002 " + nick + " :Your host is " + servername); }

static string  Numerics::RPL_CREATED(const string& server, const string& nick, const string& datetime) {
    return (":" + server + " 003 " + nick + " :This server was created " + datetime); }

static string  Numerics::RPL_MOTD(const string& server, const string& nick, const string& line) {
    return (":" + server + " 372 " + nick + " :" + line); }

static string  Numerics::RPL_MOTDSTART(const string& server, const string& nick, const string& info) {
    return (":" + server + " 375 " + nick + " :" + info); }

static string  Numerics::RPL_ENDOFMOTD(const string& server, const string& nick) {
    return (":" + server + " 376 " + nick + ":End of /MOTD command"); }

static string  Numerics::ERR_NOMOTD(const string& server, const string& nick) {
    return (":" + server + " 422 " + nick + ":MOTD File is missing"); }

static string  Numerics::RPL_YOUREOPER(const string& server, const string& nick) {
    return (":" + server + " 381 " + nick + ":You are now an IRC operator"); }

static string  Numerics::ERR_NOPRIVILEGES(const string& server, const string& nick) {
    return (":" + server + " 481 " + nick + ":Permission denied - You are not an IRC operator"); }

static string  Numerics::ERR_NOPRIVS(const string& server, const string& nick, const string& cmd) {
    return (":" + server + " 723 " + nick + " " + cmd + ":Insufficient oper privileges"); }

Numerics::Numerics() {}
Numerics::Numerics(const Numerics&) {}
Numerics&    Numerics::operator=(const Numerics&) { return (*this); }
