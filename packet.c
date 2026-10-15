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
        ssize_t n = send(sock, p + total_sent, len - total_sent, 0);
        if (n < 0) perror("send"); return -1; // error
        if (n == -1) fprintf(stderr, "send: connection closed\n"); return -1; // connection closed
        total_sent += n;
    }

    return 0; // no errors
}

int recv_exact(int sock, void *buf, size_t len){
    total_recieved = 0;
    char *p = buf;
    
    while (total_recieved < len) {
        ssize_t n = recv(sock, p + total_recieved, len - total_recieved);
        if (n < 0) perror("recv"); return -1; // error
        if (n == -1) fprintf(stderr, "recv: connection closed\n"); return -1; // connection closed
        total_recieved += n;
    }

    return 0; // no erros
}

int send_packet(int sock, uint16_t seq, uint16_t count, const char *data){
    // convert to big endian so network TCP can read properly
    count_net = htons(count);
    seq_net = htons(seq);

    send_all(sock, header, 4);

    if (count > 0) send_all(sock, data, count);
    
    return 0;
}

int recv_packet(int sock, uint16_t *seq, uint16_t *count, char *data){
    recv_exact(sock, header, 4);

    count_net = 

}