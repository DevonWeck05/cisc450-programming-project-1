#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 9000

int main() {
    /*
        Create socket
        AF_INET: Tell OS I want an IPv4 socket
        SOCK_STREAM: Tell OS I want reliable data transfer => TCP socket
        0: use default protocal
    */
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1); // couldn't create the socket
    }

    /*
        Bind to a port
        sockaddr_in: socket address, internet version
        sin_family: this socket uses IPv4
        sin_port: sets port number for server to listen on (hardcoded at top)
        sin_addr.s_addr: bind to all available network interfaces.
    */
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    // Bind sock to addr
    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sock);
        exit(1);
    }

    // Listen on socket (start waiting for clients)
    if (listen(sock, 1) < 0) {
        perror("listen");
        close(sock);
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    // ================================
    // THE PROGRAM WAITS FOR A CLIENT 
    // ================================

    // Accept a client
    int client_sock = accept(sock, NULL, NULL); // This accept() is actually where the program hangs untill there is a client
    if (client_sock < 0) {
        perror("accept");
        close(sock);
        exit(1);
    }

    // 5. Print confirmation
    printf("Client connected!\n");

    // 6. Close connection
    close(client_sock);
    close(sock);

    // 7. Exit
    return 0;
}
