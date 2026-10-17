#include "packet.h"
#include <stdio.h>       // printf(), fgets(), fopen(), fwrite()
#include <string.h>      // strlen(), strcspn()
#include <unistd.h>      // close()
#include <arpa/inet.h>   // inet_pton(), htons()
#include <sys/socket.h>  // socket(), connect()
#include <signal.h>      // signal(), SIGPIPE

#define PORT 9000
#define SERVER_IP "127.0.0.1"
#define OUTPUT_FILE "out.txt"    // these two includes pretty much just say, connect to local port 9000 and save the file as out.txt

int main(void) {
    // the "+ 2" helps us account for the enter (\n) and \0 denoting the end of user input strings
    char filename[MAX_DATA + 2];

    // prompts user for a filename
    printf("Enter the filename to request: ");
    fflush(stdout);
    // actually reads the filename input from the user, accounting for invalid filenames
    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        fprintf(stderr, "Could not read filename\n");
        return 1;
    }
    // consider all input except for enter at the end (\n)
    filename[strcspn(filename, "\r\n")] = '\0';
    size_t filename_len = strlen(filename);
    // handles empty inputs, as well as inputs that are too large
    if (filename_len == 0 || filename_len > MAX_DATA) {
        fprintf(stderr, "Filename must contain 1 to %d bytes\n", MAX_DATA);
        return 1;
    }
    // ignore signal so that process does not terminate prematurely
    signal(SIGPIPE, SIG_IGN);
    // basic socket creation, AF_INET tells it to use an IPv4 address, SOCK_STREAm indicates iTCP, and 0 marks default
    // a socket less than 0 indicates socket creation failure
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    // initialize an empty struct to hold IPv4 address and port # (htons converts port # into network byte order)
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    // fill in server's IPv4 addrewss and port
    if (inet_pton(AF_INET, SERVER_IP, &addr.sin_addr) != 1) {
        fprintf(stderr, "Invalid server address\n");
        close(sock);
        return 1;
    }
    // establish TCP connection, once again, value less than 0 denotes failure
    // passes in the socket we created, the server address, and the size of the address struct
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return 1;
    }
    // TCP connection does not fail (>0) meaning the connection was successful
    printf("Connected to server\n");

    // sends packet with the established connection, sequence # 0, filename length, and filename bytes (determined by user input)
    if (send_packet(sock, 0, (uint16_t)filename_len, filename) < 0) {
        fprintf(stderr, "Error sending filename packet\n");
        close(sock);
        return 1;
    }

    /*
        recv_packet() currently trusts the count in the incoming header.
        Reserve room for any 16-bit count so it cannot overflow our buffer;
        below, reject packets exceeding the protocol's 80-byte limit.
    */

    char data[UINT16_MAX];
    uint16_t seq, count;
    uint16_t expected_seq = 1;
    size_t total_packets = 0;
    size_t total_bytes = 0;
    FILE *out = NULL;
    int status = 1;

    while (1) {
        // reads a packet using recv_packet
        if (recv_packet(sock, &seq, &count, data) < 0) {
            fprintf(stderr, "Transfer failed before end of transmission\n");
            break;
        }
        // checks for validity of incoming packet based on size and sequence #
        if (count > MAX_DATA || seq != expected_seq) {
            fprintf(stderr, "Invalid packet: sequence %u, data bytes %u\n",
                    (unsigned)seq, (unsigned)count);
            break;
        }

        // open out.txt once after receiving the first valid response
        if (out == NULL) {
            out = fopen(OUTPUT_FILE, "wb");
            if (out == NULL) {
                perror("open output file");
                break;
            }
        }

        // a 0 byte packet indicates the end of transfer
        if (count == 0) {
            printf("End of Transmission Packet with sequence number %u received\n",
                   (unsigned)seq);
            status = 0;
            break;
        }

        // write the correct amount of bytes to a file based on packet size
        if (fwrite(data, 1, count, out) != count) {
            perror("write output file");
            break;
        }
        // count the packet and its bytes, and increment expected seq #
        printf("Packet %u received with %u data bytes\n",
               (unsigned)seq, (unsigned)count);

        total_packets++;
        total_bytes += count;
        expected_seq++;
    }

    // stop writing and close the file, then close the connection
    if (out != NULL && fclose(out) != 0) {
        perror("close output file");
        status = 1;
    }
    close(sock);

    // print the result of the transfer, and whether or not the program succeeded
    if (status == 0) {
        printf("Number of data packets received: %zu\n", total_packets);
        printf("Total number of data bytes received: %zu\n", total_bytes);
        printf("File saved to %s\n", OUTPUT_FILE);
    } else if (out != NULL) {
        fprintf(stderr, "%s may contain an incomplete transfer\n", OUTPUT_FILE);
    }

    return status;
}
