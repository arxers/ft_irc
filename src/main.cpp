#include "../inc/Server.hpp"

volatile sig_atomic_t running = false;

static void signal_handler(int) {
    running = false;
    std::cout << '\n';
}

int main(int ac, char** av) {
    if (ac != 3) {
        std::cerr << "Usage: ./ircserv <port> <password>\n";
        return (EXIT_FAILURE);
    }

    std::string port(av[1]);
    std::string password(av[2]);

    signal(SIGINT, signal_handler);
    try {
        Server server(port, password);
        server.run();
        
    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }
}