#include "packet.h"
#include <stdint.h> // uint16_t
#include <unistd.h> // read(), write()
#include <sys/socket.h> // send(), recv()
#include <string.h> // memcpy()
#include <arpa/inet.h> // htons() and ntohs()
#include <stdio.h> // error printing

// Keep sending until all bytes are sent
int send_all(int sock, const void *buf, size_t len){
    total_sent = 0;
    while (total_sent < len) {
        n = send(sock, buf + total_sent, len - total_sent, 0);
        if (n < 0) return -1; // error
        if (n == -1) return -1; // connection closed
        total_sent += n;
    }
    return 0; // no errors
}

int recv_exact(int sock, void *buf, size_t len){
    total_recieved = 0;
    while (total_recieved < len) {
        n = recv(sock, buf + total_recieved, len - total_recieved);
        if (n < 0) return -1; // error
        if (n == -1) return -1; // connection closed
        total_recieved += n;
    }
    return 0; // no erros
}

int send_packet(int sock, uint16_t seq, uint16_t count, const char *data){
    // convert to big endian so network TCP can read properly
    count_net = htons(count);
    seq_net = htons(seq);

    
}

int recv_packet(int sock, uint16_t *seq, uint16_t *count, char *data){

}