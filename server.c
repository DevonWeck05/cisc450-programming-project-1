#include "packet.h"
#include <stdio.h> // prints and file i/o
#include <stdlib.h> // exit on errors
#include <unistd.h> // closing sockets
#include <string.h> // NULL and strlen
#include <arpa/inet.h> // htons and ntohs and IPv4 addresses
#include <sys/socket.h> // socket/bind/listen/accept/send/recv

#define PORT 9000

int main() {
    /*
        Create socket
        socket():
            AF_INET: Tell OS I want an IPv4 socket
            SOCK_STREAM: Tell OS I want reliable data transfer => TCP socket
            0: use default protocal
    */
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1); // couldn't create the socket
    }

    // Even if the port is in TIME_WAIT, let me bind again.
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /*
        Bind to a port
        sockaddr_in addr: socket address, internet version
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

    // ==========================
    // PROGRAM WAITS FOR A CLIENT 
    // ==========================

    /*
        Accept a client
        When a client calls connect on this socket, it does the TCP 3 way handshake and wakes up the server.
        Then we have a new socket that is created specifically for talking to the client (client_sock)
        accept(): returns the new socket dedicated to that client
            sock: the listening socket
            NULL: we dont care what IP address we are connected to, so dont store it
            NULL: we dont care about the IP address so dont give us its length either
    */
    int client_sock = accept(sock, NULL, NULL); // This accept() is actually where the program hangs untill there is a client
    if (client_sock < 0) {
        perror("accept");
        close(sock);
        exit(1);
    }
    printf("Client connected!\n");

    // Buffer for filename
    char filename[81]; // 80 bytes + '\0'
    uint16_t seq, count;

    /*
        Receive the filename packet (seq should be 0)
        filename gets passed in as the data field for recv_packet
        using pointer logic:
            filename will have the name of the file
            seq will have the seq number of the packet 
            count will have the specified number of bytes in the data field of the packet
    */
    if (recv_packet(client_sock, &seq, &count, filename) < 0) {
        fprintf(stderr, "Error receiving filename packet\n");
        close(client_sock);
        close(sock);
        exit(1);
    }

    // Null-terminate the filename (client sends no '\0')
    filename[count] = '\0';
    printf("Filename received: %s\n", filename);

    // Open the requested file
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Could not open file: %s\n", filename);
        close(client_sock);
        close(sock);
        exit(1);
    }
    printf("File opened successfully.\n");

    uint16_t seq_num = 1; // sequence numbers start at 1 for data packets
    int total_packets = 0; // for statistics
    int total_bytes = 0; // for statistics

    // MAX_DATA = 80 (bytes)
    char data_buf[MAX_DATA + 1]; // +1 for safety if you want to null-terminate

    /*
        fgets(): read one line of file
            data_buf: where to write the data from the line in file
            sizeof(data_buf): how big data_buf is (so 81 bytes)
            fp: the file we are reading from
    */
    while (fgets(data_buf, sizeof(data_buf), fp) != NULL) {
        // How many bytes/characters we read fron the file (including "\n")
        int data_len = strlen(data_buf);

        if (send_packet(client_sock, seq_num, data_len, data_buf) < 0) {
            fprintf(stderr, "Error sending data packet\n");
            fclose(fp);
            close(client_sock);
            close(sock);
            exit(1);
        }

        printf("Packet %d transmitted with %d data bytes\n", seq_num, data_len);

        total_packets++;
        total_bytes += data_len;
        seq_num++;
    }

    // Send EOT packet
    if (send_packet(client_sock, seq_num, 0, "") < 0) {
        fprintf(stderr, "Error sending EOT packet\n");
        fclose(fp);
        close(client_sock);
        close(sock);
        exit(1);
    }
    printf("End of Transmission Packet with sequence number %d transmitted\n", seq_num);

    // Print statistics
    printf("Number of data packets transmitted: %d\n", total_packets);
    printf("Total number of data bytes transmitted: %d\n", total_bytes);

    // Close connections
    fclose(fp);
    close(client_sock);
    close(sock);

    return 0;
}
