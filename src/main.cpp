#include "../inc/Server.hpp"

volatile sig_atomic_t running = false;

static void signal_handler(int) {
    running = false;
    std::cout << '\n';
}

int main(int ac, char** av) {
    if (ac != 3) {
        cerr << "Usage: ./ircserv <port> <password>\n";
        return (EXIT_FAILURE);
    }

    string port(av[1]);
    string password(av[2]);

    signal(SIGINT, signal_handler);
    
    try {
        Server server;
        server.init("poopoo", port, password);
        server.start();
        
    } catch (std::exception& e) {
        cerr << "Error: " << e.what() << '\n';
    }
}