#include "../inc/Server.hpp"

int main(int ac, char** av) {
    if (ac != 3) {
        std::cerr << "Usage: ./ircserv <port> <password>\n";
        return (EXIT_FAILURE);
    }

    int         port = std::atoi(av[1]);
    std::string password(av[2]);
}