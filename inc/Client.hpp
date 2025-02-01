#pragma once

class Client {
private:
    Client();
    Client(const Client& rhs);
    Client& operator=(const Client& rhs);
public:
    ~Client();
};
