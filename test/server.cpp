#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 256

int client_sockets[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* handle_client(void* client_socket) {
    int sock = *(int*)client_socket;
    char buffer[BUFFER_SIZE];
    int n;

    while ((n = recv(sock, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[n] = '\0';
        std::cout << "Received message: " << buffer << std::endl;

        // Broadcast the message to all connected clients
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < client_count; ++i) {
            if (client_sockets[i] != sock) { // Don't send back to the sender
                send(client_sockets[i], buffer, n, 0);
            }
        }
        pthread_mutex_unlock(&mutex);
    }

    // Remove client from the list and close the socket
    close(sock);
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < client_count; ++i) {
        if (client_sockets[i] == sock) {
            client_sockets[i] = client_sockets[--client_count];
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main(int ac, char** av) {
    if (ac != 2) {
        std::cerr << "Usage: ./server <port>\n";
        return (1);
    }
    int port = std::atoi(av[1]);
    int sockfd, newsockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "ERROR opening socket" << std::endl;
        return 1;
    }

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        throw std::runtime_error("setsockopt() failed");

    memset((char*)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "ERROR on binding" << std::endl;
        return 1;
    }

    listen(sockfd, MAX_CLIENTS);
    clilen = sizeof(cli_addr);

    std::cout << "Server listening on port " << port << std::endl;

    while (true) {
        newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
        if (newsockfd < 0) {
            std::cerr << "ERROR on accept" << std::endl;
            continue;
        }

        pthread_mutex_lock(&mutex);
        if (client_count < MAX_CLIENTS) {
            client_sockets[client_count++] = newsockfd;

            pthread_t thread_id;
            pthread_create(&thread_id, NULL, handle_client, (void*)&newsockfd);
            pthread_detach(thread_id); // Detach the thread
        } else {
            std::cerr << "Max clients reached. Connection refused." << std::endl;
            close(newsockfd);
        }
        pthread_mutex_unlock(&mutex);
    }

    close(sockfd);
    return 0;
}
