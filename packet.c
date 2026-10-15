#include "packet.h"
#include <stdint.h> // uint16_t => int that is 16 bits, or 2 bytes and unsigned
#include <unistd.h> // read(), write()
#include <sys/socket.h> // send(), recv()
#include <string.h> // memcpy()
#include <arpa/inet.h> // htons() and ntohs()
#include <stdio.h> // error printing

// Keep sending until all bytes are sent
int send_all(int sock, const void *buf, size_t len){
    size_t total_sent = 0;
    const char *p = buf;

    while (total_sent < len) {
        /*
            sock: the connected socket we are sending to
            p + total_sent: where to start sending
                start sending from the position after the bytes we already sent
            len - total_sent: how much is left
                ensures you never send more than what’s left
            0: flags but there is none
        */
        ssize_t n = send(sock, p + total_sent, len - total_sent, 0);
        if (n < 0) {
            perror("send");
            return -1; // error
        }
        if (n == 0) {
            fprintf(stderr, "send: connection closed\n");
            return -1; // connection closed
        }
        total_sent += n;
    }

    return 0; // no errors
}

int send_packet(int sock, uint16_t seq, uint16_t count, const char *data){
    // convert to big endian so network TCP can read properly
    uint16_t count_net = htons(count);
    uint16_t seq_net = htons(seq);

    // make an array that stores the header information in 4 bytes (char = 1 byte)
    unsigned char header[4];
    memcpy(header, &count_net, 2);
    memcpy(header + 2, &seq_net, 2);

    // Send the header first
    if (send_all(sock, header, 4) < 0) return -1;
    // Then send the data if header sends good
    if (count > 0) {
        if (send_all(sock, data, count) < 0) return -1;
    }
    
    return 0;
}

int recv_exact(int sock, void *buf, size_t len){
    size_t total_received = 0;
    char *p = buf;
    
    while (total_received < len) {
        /*
            sock: the connected socket we are reciving from
            p + total_sent: where to start filling buffer
                start filling the buffer after the bytes we already filled
            len - total_sent: how many bytes we still need to recive
                ensures you never recieve more than what’s left
            0: flags but there is none
        */
        ssize_t n = recv(sock, p + total_received, len - total_received);
        if (n < 0) {
            perror("recv");
            return -1; // error
        }
        if (n == 0) {
            fprintf(stderr, "recv: connection closed\n");
            return -1; // connection closed
        }
        total_received += n;
    }

    return 0; // no erros
}

int recv_packet(int sock, uint16_t *seq, uint16_t *count, char *data){
    // make space for recieving header
    unsigned char header[4];

    // receive the header and error if error
    if (recv_exact(sock, header, 4) < 0) return -1;

    // copy the info in header out of header
    uint16_t count_net, seq_net;
    memcpy(&count_net, header, 2);
    memcpy(&seq_net, header + 2, 2);

    // convert back to little endian so our machine can read it properly
    *count = ntohs(count_net);
    *seq = ntohs(seq_net);

    // revieve the data portion
    if (*count > 0) {
        if (recv_exact(sock, data, *count) < 0) return -1;
    }

    return 0;
}