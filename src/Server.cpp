#include "../inc/Server.hpp"

int Server::_createSocket() {
    struct addrinfo hints = {}, *res, *p;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo(NULL, this->_port.c_str(), &hints, &res) == -1)
        throw std::runtime_error("_createSocket: getaddrinfo: Failed to get address info");

    int socketFd = -1;
    for (p = res; p != NULL; p = p->ai_next) {
        socketFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (socketFd == -1)
            continue ;

        int opt = 1;
        if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
            cerr << "_createSocket: setsockopt() failed\n";
            continue ;
        }

        if (bind(socketFd, p->ai_addr, p->ai_addrlen) == -1) {
            close(socketFd);
            continue ;
        }
        break ;
    }
    if (socketFd == -1) {
        freeaddrinfo(res);
        throw std::runtime_error("_createSocket: Failed to bind socket");
    }

    if (listen(socketFd, MAX_CLIENTS) == -1) {
        close(socketFd);
        freeaddrinfo(res);
        throw std::runtime_error("_createSocket: listen() failed");
    }

    freeaddrinfo(res);
    return (socketFd);
}

void    Server::_addClient() {
    sockaddr_in clientAddr;
    socklen_t   clientLen = sizeof(clientAddr);

    int clientFd = accept(this->_listeningSocket, (struct sockaddr*)&clientAddr, &clientLen);
    if (clientFd == -1)
        return ;

    if (this->_clientCount < MAX_CLIENTS) {
        this->_clientCount++;
        Client newClient(clientFd, clientAddr);
        if (this->_password.empty())
            newClient.setState(AUTHENTICATED);
        this->_clients[clientFd] = newClient;
        struct pollfd   client_pfd = {clientFd, POLLIN | POLLOUT, 0};
        this->_pollFds.push_back(client_pfd);
        cout << newClient.getIp() <<  " connected to socket FD: "
             << clientFd << '\n';
        return ;
    }
    close(clientFd);
}

void    Server::_removeClient(int socketFd) {
    cout << this->_clients[socketFd].getIp() << " disconnected from socket FD: " << socketFd << '\n';
    for (vector<pollfd>::iterator it = this->_pollFds.begin(); it != this->_pollFds.end(); ++it) {
        if (it->fd == socketFd) {
            this->_pollFds.erase(it);
            break ;
        }
    }
    this->_clientCount--;
    this->_clients.erase(socketFd);
    close(socketFd);
}

void    Server::_handleClient(int clientFd) {
    char            buf[MAX_MSG_LEN + 1];
    int             receivedBytes;

    receivedBytes = recv(clientFd, buf, sizeof(buf), 0);
    if (receivedBytes == 0) {
        this->_disconnecting.push_back(clientFd);
        return ;
    }

    Client& client = this->_clients[clientFd];
    client.setLastActiveTime();

    string &inputBuffer = client.getInputBuffer();
    inputBuffer.append(buf, receivedBytes);
    if (inputBuffer.length() > MAX_MSG_LEN) {
        inputBuffer = inputBuffer.substr(0, MAX_MSG_LEN - 2);
        inputBuffer += "\r\n";
    }
    
    string &outputBuffer = client.getOutputBuffer();
    while (inputBuffer.find("\r\n") != string::npos) {
        size_t  pos = inputBuffer.find("\r\n");
        string input = inputBuffer.substr(0, pos);
        outputBuffer += _generateResponse(client, input);
        cout << "<" << clientFd << ": " << input << '\n';
        inputBuffer.erase(0, pos + 2);
    }
}

static string  strToUpper(string s) {
    for (string::iterator it = s.begin(); it != s.end(); ++it)
        *it = (std::toupper(*it));
    return (s);
}

string Server::_cap(const vector<string>& params) {
    if (params.empty() || params[0] != "LS")
        return ("");
    return (":" + this->_name + " " + "CAP * LS :" + CRLF);
}

string  Server::_pass(Client& client, const vector<string>& params) {
    if (client.isAuthenticated())
        return (Numerics::formatMessage(this->_name, ERR_ALREADYREGISTERED, client.getNickname(), "Unauthorized command (already registered)"));
    if (params.size() < 1)
        return (Numerics::formatMessage(this->_name, ERR_NEEDMOREPARAMS, client.getNickname(), "Not enough parameters"));
    if (params[0] == this->_password) {
        client.setState(AUTHENTICATED);
        return ("");
    }
    this->_disconnecting.push_back(client.getSocket());
    string  reply;
    reply += Numerics::formatMessage(this->_name, ERR_PASSWDMISMATCH, client.getNickname(), "Password incorrect");
    reply += "ERROR :Closing Link: " + client.getIp() + " (Incorrect Password)\r\n";
    return (reply);
}

Client* Server::_getClientByNickname(const string& nickname) {
    for (clientmap_t::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
        if (it->second.getNickname() == nickname)
            return &(it->second);
    return NULL;
}

bool    Server::_isValidNickname(const string& nickname) {
    if (nickname.length() > 9)
        return (false);

    string letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    string digits = "1234567890";
    string special = "[]\\_^{}`";

    if (letters.find(nickname[0]) == string::npos &&
        special.find(nickname[0]) == string::npos)
        return (false);

    for (size_t i = 1; i < nickname.length(); ++i) {
        if (letters.find(nickname[i]) == string::npos &&
            special.find(nickname[i]) == string::npos &&
            digits.find(nickname[i]) == string::npos &&
            nickname[i] != '-')
            return (false);
    }

    return (true);
}

string  Server::_sendWelcomeBurst(Client& client) {
    client.setState(REGISTERED);
    client.setLastPingTime();
    return (Numerics::formatMessage(this->_name, RPL_WELCOME, client.getNickname(), "Welcome to " + this->_name + ", " + client.getNickname()));
}

string  Server::_nick(Client& client, const vector<string>& params) {
    if (params.size() < 1)
        return (Numerics::formatMessage(this->_name, ERR_NONICKNAMEGIVEN, client.getNickname(), "No nickname given"));
    if (!_isValidNickname(params[0]))
        return (Numerics::formatMessage(this->_name, ERR_ERRONEUSNICKNAME, client.getNickname(), params[0], "Erroneous nickname"));
    if (_getClientByNickname(params[0]))
        return (Numerics::formatMessage(this->_name, ERR_NICKNAMEINUSE, client.getNickname(), params[0], "Nickname is already in use"));
    client.setNickname(params[0]);
    if (client.getState() == AUTHENTICATED && client.getUsername() != "")
        return (_sendWelcomeBurst(client));
    return ("");
}

string  Server::_user(Client& client, const vector<string>& params) {
    if (params.size() < 4)
        return (Numerics::formatMessage(this->_name, ERR_NEEDMOREPARAMS, client.getNickname(), "Not enough parameters"));
    client.setUsername(params[0]);
    client.setRealname(params[3]);
    if (client.getState() == AUTHENTICATED && client.getNickname() != "*")
        return (_sendWelcomeBurst(client));
    return ("");
}

bool    isValidChannelName(const std::string& channel) {
    if (channel.empty() || channel.length() < 2)
        return (false);
    if (channel[0] != '#')
        return (false);
    static const string invalidChars(" ,:");
    for (size_t i = 1; i < channel.length(); ++i)
        if (!std::isprint(channel[i]) || invalidChars.find(channel[i]) != string::npos)
            return (false);
    return (true);
}

string  Server::_join(Client& client, const vector<string>& params) {
    if (params.empty() || params.size() < 1)
        return (Numerics::formatMessage(this->_name, ERR_NEEDMOREPARAMS, client.getNickname(), "Not enough parameters"));

    // make channel map from input params
    std::istringstream  issChannels(params[0]);
    std::istringstream  issKeys(params.size() >= 2 ? params[1] : "");
    string  channelStr, keyStr;
    vector< pair<string, string> > channelKeyMap;
    while (std::getline(issChannels, channelStr, ',')) {
        if (!std::getline(issKeys, keyStr, ','))
            keyStr = "";
        channelKeyMap.push_back(std::make_pair(channelStr, keyStr));
    }

    string reply;
    for (size_t i = 0; i < channelKeyMap.size(); ++i) {
        if (!isValidChannelName(channelKeyMap[i].first)) {
            reply += Numerics::formatMessage(this->_name, ERR_NOSUCHCHANNEL, client.getNickname(), channelKeyMap[i].first, "No such channel");
            continue ;
        } //if channel already exists
        if (this->_channels.find(channelKeyMap[i].first) != this->_channels.end()) {
            if (this->_channels[channelKeyMap[i].first].addClient(&client, channelKeyMap[i].second) == -1)
            reply += Numerics::formatMessage(this->_name, ERR_BADCHANNELKEY, client.getNickname(), channelKeyMap[i].first, "Cannot join channel (+k)");
        } else { //else create channel
            Channel newChannel(channelKeyMap[i].first, client);
            this->_channels[channelKeyMap[i].first] = newChannel;
        }
    }
    return (reply);
}

string  Server::_part(Client& client, const vector<string>& params) {
    if (params.size() < 1)
        return (Numerics::formatMessage(this->_name, ERR_NEEDMOREPARAMS, client.getNickname(), "Not enough parameters"));

    std::istringstream  issChannels(params[0]);
    string  channelStr;
    vector<string> partingChannels;
    while (std::getline(issChannels, channelStr, ','))
        partingChannels.push_back(channelStr);
    
    string  message = (params.size() > 1 ? params[1] : client.getNickname());
    string  reply;
    for (vector<string>::iterator channelName = partingChannels.begin(); channelName != partingChannels.end(); ++channelName) {
        vector<string> clientChannels = client.getChannels();
        if (this->_channels.find(*channelName) == this->_channels.end()) {
            reply += Numerics::formatMessage(this->_name, ERR_NOSUCHCHANNEL, client.getNickname(), *channelName, "No such channel");
            continue ;
        }
        if (!client.isInChannel(*channelName)) {
            reply += Numerics::formatMessage(this->_name, ERR_NOTONCHANNEL, client.getNickname(), *channelName, "You're not on that channel");
            continue ;
        }
        Channel&    channel = this->_channels[*channelName];
        channel.broadcastMessage(message, "PART", client.getNickname());
        channel.removeClient(client);
    }
    return (reply);
}

string  Server::_privMsg(Client& client, const vector<string>& params) {
    if (params.empty())
    return (Numerics::formatMessage(this->_name, ERR_NORECIPIENT, client.getNickname(), "No recipient given (PRIVMSG)"));
    
    if (params.size() == 1)
    return (Numerics::formatMessage(this->_name, ERR_NOTEXTTOSEND, client.getNickname(), "No text to send"));
    
    if (params[0][0] == '#') { // if target is a channel
        channelmap_t::iterator it = this->_channels.find(params[0]);
        if (it == this->_channels.end())
            return (Numerics::formatMessage(this->_name, ERR_NOSUCHNICK, client.getNickname(), "No such nick/channel"));
        if (!client.isInChannel(params[0]))
            return (Numerics::formatMessage(this->_name, ERR_CANNOTSENDTOCHAN, client.getNickname(), params[0], "Cannot send to channel"));
        it->second.broadcastMessage(params[1], "PRIVMSG", client);
        return ("");
    }

    // else if target is a client
    Client* targetClient = _getClientByNickname(params[0]);
    if (!targetClient)
        return (Numerics::formatMessage(this->_name, ERR_NOSUCHNICK, client.getNickname(), "No such nick/channel"));

    targetClient->sendMessage(params[1], client.getNickname());
    return ("");
}
/*
join #minishell
:jerlim!~j@203.149.201.178 JOIN #minishell

Client OP:
KICK #minishell whoarr
:jerlim!~j@203.149.201.178 KICK #minishell whoarr :whoarr

Client User:
join #minishell
:whoarr!~a@203.149.201.178 JOIN #minishell
:platinum.libera.chat 353 whoarr @ #minishell :whoarr @jerlim
:platinum.libera.chat 366 whoarr #minishell :End of /NAMES list.
:jerlim!~j@203.149.201.178 KICK #minishell whoarr :whoarr

KICK #minishell whoarr :Memleaks!
:jerlim!~j@203.149.201.178 KICK #minishell whoarr :Memleaks! <-- Operator
:jerlim!~j@203.149.201.178 KICK #minishell whoarr :Memleaks! <-- User

Server broadcasts the message to the clients in the room

 */

typedef vector< pair<string, string> > StringPairs;

StringPairs stringPairs(const string& str1, const string& str2) {
    std::istringstream  issKeys(str1);
    std::istringstream  issValues(str2);
    string  keyStr, valueStr;
    vector< pair<string, string> > sp;
    while (std::getline(issKeys, keyStr, ',')) {
        if (!std::getline(issValues, valueStr, ','))
            valueStr = "";
        sp.push_back(std::make_pair(keyStr, valueStr));
    }
    return sp;
}

string  Server::_kick(Client& client, const vector<string>& params) {
    if (params.size() < 2)
        return (Numerics::formatMessage(this->_name, ERR_NEEDMOREPARAMS, client.getNickname(), "Not enough parameters"));
    StringPairs channelUserPairs = stringPairs(params[0], params[1]);
    // make channel map from input params
    // Map channel name to nickname


    // For channel in list of channels
    for (StringPairs::iterator it = channelUserPairs.begin(); it != channelUserPairs.end(); ++it) {
        if (it->first[0] == '#') { // if target is a channel
            // valid channel?
            channelmap_t::iterator itChan = this->_channels.find(params[0]);
            if (itChan == this->_channels.end())
                return (Numerics::formatMessage(this->_name, ERR_NOSUCHCHANNEL, client.getNickname(), "No such channel"));
            if (!client.isInChannel(it->first))
                return (Numerics::formatMessage(this->_name, ERR_NOTONCHANNEL, client.getNickname(), params[0], "You're not on that channel"));

            // valid target user?
            Client* targetClient = this->_getClientByNickname(it->second);
            if (targetClient == NULL)
                return (Numerics::formatMessage(this->_name, ERR_NOSUCHNICK, it->second, "No such nick/channel"));
            //itChan->second.broadcastMessage(Numerics::formatMessage(this->_name, ), client);
            cout << "KICK" + it->first + " " + it->second + ":" + client.getNickname();
            return ("");
        }

        // Check if client is op in the desired channel?

            // forcefully part target user


        // :jerlim!~j@203.149.201.178 KICK #hello jerlim :jerlim
    // <prefix><~ indicates non identified user by ident><username>@<ip_addr> KICK <channel> <nick> :<nick of kicker | kick msg>
    //cout << "KICK" + " #channelA, #channelB " + "nickname"
    
    }
    return ("");
}

string Server::_ping(Client& client, const vector<string>& params) {
    (void)client;
    if (params.empty())
        return (":" + this->_name + " PONG " + this->_name + " :" + CRLF);
    return (":" + this->_name + " PONG " + this->_name + " :" + params[0] + CRLF);
}
string Server::_pong(Client& client, const vector<string>& params) {
    (void)params;
    client.setLastPingTime();
    client.setPinged(false);
    return ("");
}
string Server::_quit(Client& client, const vector<string>& params) {
  (void)client;
  (void)params;
  return ("");
}

string Server::_generateResponse(Client& client, Message message) {
    string commandUpper = strToUpper(message.getCommand());
    commandmap_t::iterator   it = this->_commands.find(commandUpper);

    string reply;
    e_command command = it->second;
    const vector<string>&   params = message.getParams();
    if (command == CAP)
        return (_cap(params));

    if (client.getState() == CONNECTED) {
        if (command != PASS)
            return Numerics::formatMessage(this->_name, ERR_PASSWDMISMATCH, "*", "Password required");
    }
    else if (client.getState() < REGISTERED) {
        if (command != NICK && command != USER)
            return Numerics::formatMessage(this->_name, ERR_NOTREGISTERED, "*", "You have not registered");
    }

    switch (command) {
        case PASS:      return (_pass(client, params));
        case NICK:      return (_nick(client, params));
        case USER:      return (_user(client, params));
        case PART:      return (_part(client, params));
        case JOIN:      return (_join(client, params));
        case PRIVMSG:   return (_privMsg(client, params));
        case KICK:      return (_kick(client, params));
        case INVITE:    return ("INVITE\n");
        case TOPIC:     return ("TOPIC\n");
        case MODE:      return ("MODE\n");
        case PING:      return (_ping(client, params));
        case PONG:      return (_pong(client, params));
        case QUIT:      return (_quit(client, params));
        default: return (Numerics::formatMessage(this->_name, ERR_UNKNOWNCOMMAND, client.getNickname(), message.getCommand(), "Unknown command!"));
    }
}

void    Server::_flushClientBuffer(Client& client) {
    int fd = client.getSocket();
    string& buf = client.getOutputBuffer();
    size_t  size = buf.size();

    if (!size)
        return ;
    cout << ">" << client.getSocket() << ": " << buf;
    send(fd, buf.c_str(), size, MSG_NOSIGNAL);
    buf.clear();
}

static string toString(int num) {
    std::ostringstream oss;
    oss << num;
    return oss.str();
}

void    Server::start() {
    if (this->_name.empty())
        throw (std::runtime_error("start: Server not initialized"));

    this->_listeningSocket = _createSocket();
    cout << "Server " << this->_name << " started successfully!\n"
         << "Listening on port " << this->_port << "...\n"
         << "Waiting for client connections...\n";

    running = true;
    struct pollfd   server_pfd = {this->_listeningSocket, POLLIN, 0};
    this->_pollFds.push_back(server_pfd);

    while (running) {
        int pollResult = poll(this->_pollFds.data(), this->_pollFds.size(), -1);
        if (pollResult == -1) {
            break ;
        }
        for (size_t i = 0; i < this->_pollFds.size(); ++i) {
            if (this->_pollFds[i].revents & POLLIN) {
                if (this->_pollFds[i].fd == this->_listeningSocket)
                    _addClient();
                else
                    _handleClient(this->_pollFds[i].fd);
                }
            }
        for (clientmap_t::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it) {
            Client& client = it->second;
            if (client.isPinged() && client.getTimeSinceLastPing() >= PING_TIMEOUT) {
                string& buffer = it->second.getOutputBuffer();
                buffer += "ERROR :Closing Link: " + client.getIp() + " (Ping timeout: " + toString(PING_TIMEOUT) + "seconds)\r\n";
                this->_disconnecting.push_back(it->first);
            }
        }

        for (clientmap_t::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it) {
            Client& client = it->second;
            if (client.getState() != REGISTERED || client.isPinged() || client.getTimeSinceLastPing() < PING_TIMEOUT) {
                continue ;
            }
            string& buffer = client.getOutputBuffer();
            buffer += "PING :" + this->_name + CRLF;
            client.setLastPingTime();
            client.setPinged(true);
        }

        for (clientmap_t::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
            _flushClientBuffer(it->second);

        for (vector<int>::iterator it = this->_disconnecting.begin(); it != this->_disconnecting.end(); ++it)
            _removeClient(*it);
        this->_disconnecting.clear();
    }
    cout << "Server shutting down...\n";
}

static bool isValidPort(int n) {
    return (n >= 0 && n <= 65535);
}

static bool isNumber(const string& str) {
    return (str.find_first_not_of("1234567890") == string::npos);
}

static void initCommandMap(commandmap_t& map) {
    map["CAP"]      = CAP;
    map["PASS"]     = PASS;
    map["NICK"]     = NICK;
    map["USER"]     = USER;
    map["JOIN"]     = JOIN;
    map["PART"]     = PART;
    map["PRIVMSG"]  = PRIVMSG;
    map["KICK"]     = KICK;
    map["INVITE"]   = INVITE;
    map["TOPIC"]    = TOPIC;
    map["MODE"]     = MODE;
    map["PING"]     = PING;
    map["PONG"]     = PONG;
    map["QUIT"]     = QUIT;
}

void    Server::init(string name, string port, string password) {
    if (!isNumber(port) || !isValidPort(std::atoi(this->_port.c_str())))
        throw std::invalid_argument("init: Valid ports are 0-65535");
    this->_name = name;
    this->_port = port;
    this->_password = password;
    initCommandMap(this->_commands);
}

Server::Server() {}
Server::Server(const Server&) {}
Server& Server::operator=(const Server&) { return (*this); }
Server::~Server() {
    close(this->_listeningSocket);
    for (map<int, Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it) {
        string  message("ERROR :Closing Link: " + it->second.getIp() + " (Server shutting down)\r\n");
        send(it->first, message.c_str(), message.size(), MSG_NOSIGNAL);
        close(it->first);
    }
}