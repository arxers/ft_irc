#pragma once

#include <string>
#include <sstream>

using std::string;

#define CRLF "\r\n"

enum e_numeric {
    // 001–099: Client-server connection responses
    RPL_WELCOME = 1,             // "Welcome to the Internet Relay Network <nick>!<user>@<host>"
    RPL_YOURHOST = 2,            // "Your host is <servername>, running version <ver>"
    RPL_CREATED = 3,             // "This server was created <date>"
    RPL_MYINFO = 4,              // "<servername> <version> <available user modes> <available channel modes>"
    RPL_BOUNCE = 5,              // Sent when a client is redirected to another server

    // 200–206: Debugging responses
    RPL_TRACELINK = 200,         // Trace link information
    RPL_TRACECONNECTING = 201,
    RPL_TRACEHANDSHAKE = 202,
    RPL_TRACEUNKNOWN = 203,
    RPL_TRACEOPERATOR = 204,
    RPL_TRACEUSER = 205,
    RPL_TRACESERVER = 206,

    // 211–219: Statistics
    RPL_STATSLINKINFO = 211,     // Statistics about a connection
    RPL_STATSCOMMANDS = 212,
    RPL_STATSCLINE = 213,
    RPL_STATSNLINE = 214,
    RPL_STATSILINE = 215,
    RPL_STATSKLINE = 216,
    RPL_STATSYLINE = 218,
    RPL_ENDOFSTATS = 219,

    // 221–234: Information messages
    RPL_UMODEIS = 221,           // User mode is
    RPL_SERVLIST = 234,          // List of services
    RPL_SERVLISTEND = 235,

    // 241–250: Statistics or administration messages
    RPL_STATSLLINE = 241,
    RPL_STATSUPTIME = 242,
    RPL_STATSOLINE = 243,
    RPL_STATSHLINE = 244,
    RPL_LUSERCLIENT = 251,       // "There are <integer> users and <integer> services on <integer> servers"
    RPL_LUSEROP = 252,           // "<integer> IRC Operators online"
    RPL_LUSERUNKNOWN = 253,      // "<integer> unknown connections"
    RPL_LUSERCHANNELS = 254,     // "<integer> channels formed"
    RPL_LUSERME = 255,           // "I have <integer> clients and <integer> servers"

    // 256–259: Administrative info
    RPL_ADMINME = 256,           // "<server> :Administrative info"
    RPL_ADMINLOC1 = 257,
    RPL_ADMINLOC2 = 258,
    RPL_ADMINEMAIL = 259,

    // 261: Tracing service connections
    RPL_TRACELOG = 261,

    // 265–266: Local & global users
    RPL_LOCALUSERS = 265,        // "Current local users <int>, max <int>"
    RPL_GLOBALUSERS = 266,       // "Current global users <int>, max <int>"

    // 301–306: User-related replies
    RPL_AWAY = 301,              // "<nick> :away message"
    RPL_USERHOST = 302,
    RPL_ISON = 303,
    RPL_UNAWAY = 305,
    RPL_NOWAWAY = 306,

    // 311–318: WHOIS responses
    RPL_WHOISUSER = 311,
    RPL_WHOISSERVER = 312,
    RPL_WHOISOPERATOR = 313,
    RPL_WHOISIDLE = 317,
    RPL_ENDOFWHOIS = 318,

    // 319–324: Channel information
    RPL_WHOISCHANNELS = 319,
    RPL_LISTSTART = 321,
    RPL_LIST = 322,
    RPL_LISTEND = 323,
    RPL_CHANNELMODEIS = 324,

    // 331–341: Channel & invite responses
    RPL_NOTOPIC = 331,
    RPL_TOPIC = 332,
    RPL_INVITING = 341,
    
    // 351–353: Version & WHO responses
    RPL_VERSION = 351,
    RPL_WHOREPLY = 352,
    RPL_ENDOFWHO = 315,

    // 366–376: Channel & MOTD responses
    RPL_NAMREPLY = 353,
    RPL_ENDOFNAMES = 366,
    RPL_MOTDSTART = 375,
    RPL_MOTD = 372,
    RPL_ENDOFMOTD = 376,

    // 381–391: Admin & time-related responses
    RPL_YOUREOPER = 381,
    RPL_TIME = 391,

    // 401–406: Error messages
    ERR_NOSUCHNICK = 401,        // "<nickname> :No such nick/channel"
    ERR_NOSUCHSERVER = 402,      // "<server name> :No such server"
    ERR_NOSUCHCHANNEL = 403,     // "<channel name> :No such channel"
    ERR_CANNOTSENDTOCHAN = 404,  // "<channel name> :Cannot send to channel"
    ERR_TOOMANYCHANNELS = 405,   // "<channel name> :You have joined too many channels"
    ERR_WASNOSUCHNICK = 406,     // "<nickname> :There was no such nickname"

    // 411–415: Input errors
    ERR_NORECIPIENT = 411,       // ":No recipient given (<command>)"
    ERR_NOTEXTTOSEND = 412,      // ":No text to send"
    ERR_UNKNOWNCOMMAND = 421,    // "<command> :Unknown command"

    // 431–437: Nickname errors
    ERR_NONICKNAMEGIVEN = 431,   // ":No nickname given"
    ERR_ERRONEUSNICKNAME = 432,  // "<nick> :Erroneous nickname"
    ERR_NICKNAMEINUSE = 433,     // "<nick> :Nickname is already in use"
    ERR_NICKCOLLISION = 436,     // "<nick> :Nickname collision"

    // 441–445: Channel-related errors
    ERR_USERNOTINCHANNEL = 441,  // "<nick> <channel> :They aren't on that channel"
    ERR_NOTONCHANNEL = 442,      // "<channel> :You're not on that channel"
    ERR_USERONCHANNEL = 443,     // "<user> <channel> :is already on channel"

    // 451–467: Permissions and limit errors
    ERR_NOTREGISTERED = 451,     // ":You have not registered"
    ERR_NEEDMOREPARAMS = 461,    // "<command> :Not enough parameters"
    ERR_ALREADYREGISTERED = 462, // ":You may not reregister"
    ERR_PASSWDMISMATCH = 464,    // ":Password incorrect"
    ERR_CHANNELISFULL = 471,     // "<channel> :Cannot join channel (+l)"
    ERR_UNKNOWNMODE = 472,       // "<char> :is unknown mode char to me"
    ERR_INVITEONLYCHAN = 473,    // "<channel> :Cannot join channel (+i)"
    ERR_BANNEDFROMCHAN = 474,    // "<channel> :Cannot join channel (+b)"
    ERR_BADCHANNELKEY = 475,     // "<channel> :Cannot join channel (+k)"

    // 481–485: Operator and admin errors
    ERR_NOPRIVILEGES = 481,      // ":Permission Denied- You're not an IRC operator"
    ERR_CHANOPRIVSNEEDED = 482,  // "<channel> :You're not channel operator"
    
    // 501–502: Miscellaneous errors
    ERR_UMODEUNKNOWNFLAG = 501,  // ":Unknown MODE flag"
    ERR_USERSDONTMATCH = 502     // ":Cannot change mode for other users"
};


class Numerics {
private:
    Numerics();
    Numerics(const Numerics&);
    Numerics&    operator=(const Numerics&);
public:
    static string   formatMessage(const string& server, e_numeric num, const string& nick, const string& message) {
        std::ostringstream oss;
        oss << num;
        return (":" + server + " " + oss.str() + " " + nick + " :" + message + CRLF);
    }

    static string   formatMessage(const string& server, e_numeric num, const string& nick, const string& param, const string& message) {
        std::ostringstream oss;
        oss << num;
        return (":" + server + " " + oss.str() + " " + nick + " " + param + " :" + message  + CRLF);
    }
};
