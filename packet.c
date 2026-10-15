#include "packet.h"
#include <stdint.h> // uint16_t
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
        if (n < 0) perror("send"); return -1; // error
        if (n == -1) fprintf(stderr, "send: connection closed\n"); return -1; // connection closed
        total_sent += n;
    }

    return 0; // no errors
}

int send_packet(int sock, uint16_t seq, uint16_t count, const char *data){
    // convert to big endian so network TCP can read properly
    uint16_t count_net = htons(count);
    uint16_t seq_net = htons(seq);

    unsigned char header[4];
    memcpy(header, &count_net, 2);
    memcpy(header + 2, &seq_net, 2);

    if (send_all(sock, header, 4) < 0) return -1;
    if (count > 0) send_all(sock, data, count); return -1;
    
    return 0;
}

int recv_exact(int sock, void *buf, size_t len){
    total_recieved = 0;
    const char *p = buf;
    
    while (total_recieved < len) {
        ssize_t n = recv(sock, p + total_recieved, len - total_recieved);
        if (n < 0) perror("recv"); return -1; // error
        if (n == -1) fprintf(stderr, "recv: connection closed\n"); return -1; // connection closed
        total_recieved += n;
    }

    return 0; // no erros
}

int recv_packet(int sock, uint16_t *seq, uint16_t *count, char *data){
    unsigned char header[4];

    if (recv_exact(sock, header, 4) < 0) return -1;

    uint16_t count_net, seq_net;
    memcpy(&count_net, header, 2);
    memcpy(&seq_net, header + 2, 2);

    // Convert what the TCP sends to little endian so our machine can read it properly
    *count = ntohs(count_net);
    *seq   = ntohs(seq_net);

    if (*count > 0) {
        if (recv_exact(sock, data, *count) < 0) return -1;
    }

    return 0;
}