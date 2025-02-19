#include "../inc/Numerics.hpp"

string  Numerics::RPL_WELCOME(const string& server, const string& nick) {
    return (":" + server + " 001 " + nick + " :Welcome to the IRC Network, " + nick + CRLF); }

string  Numerics::RPL_YOURHOST(const string& server, const string& nick, const string& servername) {
    return (":" + server + " 002 " + nick + " :Your host is " + servername + CRLF); }

string  Numerics::RPL_CREATED(const string& server, const string& nick, const string& datetime) {
    return (":" + server + " 003 " + nick + " :This server was created " + datetime + CRLF); }

string  Numerics::RPL_MOTD(const string& server, const string& nick, const string& line) {
    return (":" + server + " 372 " + nick + " :" + line + CRLF); }

string  Numerics::RPL_MOTDSTART(const string& server, const string& nick, const string& info) {
    return (":" + server + " 375 " + nick + " :" + info + CRLF); }

string  Numerics::RPL_ENDOFMOTD(const string& server, const string& nick) {
    return (":" + server + " 376 " + nick + ":End of /MOTD command" + CRLF); }

string  Numerics::ERR_UNKNOWNCOMMAND(const string& server, const string& nick, const string& command) {
    return (":" + server + " 421 " + nick + " " + command + " :Unknown command" + CRLF); }

string  Numerics::ERR_NOMOTD(const string& server, const string& nick) {
    return (":" + server + " 422 " + nick + ":MOTD File is missing" + CRLF); }

string  Numerics::RPL_YOUREOPER(const string& server, const string& nick) {
    return (":" + server + " 381 " + nick + ":You are now an IRC operator" + CRLF); }

string  Numerics::ERR_NEEDMOREPARAMS(const string& server, const string& nick, const string& command) {
    return (":" + server + " 461 " + nick + " " + command + " :Not enough parameters" + CRLF); }

string  Numerics::ERR_NOPRIVILEGES(const string& server, const string& nick) {
    return (":" + server + " 481 " + nick + ":Permission denied - You are not an IRC operator" + CRLF); }

string  Numerics::ERR_NOPRIVS(const string& server, const string& nick, const string& cmd) {
    return (":" + server + " 723 " + nick + " " + cmd + ":Insufficient oper privileges" + CRLF); }

Numerics::Numerics() {}
Numerics::Numerics(const Numerics&) {}
Numerics&    Numerics::operator=(const Numerics&) { return (*this); }