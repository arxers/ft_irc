
#include "../inc/Numerics.hpp"

string   Numerics::formatMessage(const string& server, e_num num, const string& nick, const string& message) {
    std::ostringstream oss;
    oss << std::setw(3) << std::setfill('0') <<  num;
    return (":" + server + " " + oss.str() + " " + nick + " :" + message + CRLF);
}

string   Numerics::formatMessage(const string& server, e_num num, const string& nick, const string& param, const string& message) {
    std::ostringstream oss;
    oss << std::setw(3) << std::setfill('0') <<  num;
    return (":" + server + " " + oss.str() + " " + nick + " " + param + " :" + message  + CRLF);
}

string   Numerics::formatDisconnectMessage(const Client& client, const string& message) {
    return ("ERROR :Closing Link: " + client.getIp() + " (" + message + ")\r\n");
}