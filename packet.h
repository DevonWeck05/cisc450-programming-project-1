#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>

// Max bytes in a datapacket
#define MAX_DATA 80

// uint16_t looks scary but literally means an unsigned int that is 16 bits
// the number will always be positive so it can be unsigned
// 16 bits is 2 bytes, which is why we need it for the header
typedef struct {
    uint16_t count;
    uint16_t seq;
} PacketHeader;

int send_all(int sock, const void *buf, size_t len);
int send_packet(int sock, uint16_t seq, uint16_t count, const char *data);

int recv_exact(int sock, void *buf, size_t len);
int recv_packet(int sock, uint16_t *seq, uint16_t *count, char *data);

#endif