Devon Weckherlen and Achilles Demerutis

This will just be a note doc until we make the actual readme

=======================================================

What we actually need to do:
    To run the program you open 2 terminals: one for the client and one for the server
        Ex: ./server to start the server and then ./client to start the client after compiling

    The server will "have possession" of a file that the client wants to recieve. This file will be called test.txt or something (maybe requested_file.txt is better)
    The client is requesting the test.txt file from the server and it will be transfered over TCP as a series of packets
    The client writes the data from these packets into a file called out.txt (maybe recieved_file.txt is a better name)

=======================================================

Road Map: 
    Step 1: Make all necessary files
    Step 2: implement packet.c and packet.h
        #define MAX_DATA 80
        Packet header struct (2 fields: count, seq)
        packet.h - Function prototypes:
            int send_all(int sock, const void *buf, size_t len);
            int recv_exact(int sock, void *buf, size_t len);
            int send_packet(int sock, uint16_t seq, uint16_t count, const char *data);
            int recv_packet(int sock, uint16_t *seq, uint16_t *count, char *data);
        packet.c - Implement:
            send_all() — loop until all bytes sent
            recv_exact() — loop until all bytes received
            Header packing using htons()
            Header unpacking using ntohs()
            send_packet() → send header then data
            recv_packet() → read header then data
        This is the hardest part. Do it first.
    Step 3: Build the server skeleton (no file transfer logic yet)
        Create socket
        Bind to a port
        Listen
        Accept a client
        Print “Client connected” (optional)
        Close connection
        Exit
    Step 4: Build the client skeleton (no file transfer logic yet)
        Create socket
        Connect to server
        Print “Connected to server”
        Close
        Exit
    Step 5: Implement filename packet exchange
        Client:
            Prompt user for filename
            Send filename packet (seq = 0, count = strlen(filename))
        Server:
            Receive filename packet
            Open the file
            Handle errors (file not found → exit with error)
        This is the first real use of your packet library (packet.c).
    Step 6: Implement server → client data packet transmission
        Server:
            Read file line by line
            For each line:
                Count bytes (≤ 80)
                Send packet with seq starting at 1
                Print required message
            After file ends:
                Send EOT packet (count = 0)
                Print EOT message
            Print statistics
            Close connection
    Step 7: Implement client receiving loop
        Client must:
            Loop receiving packets
            If count > 0:
                Write data to out.txt
                Print required message
            If count == 0:
                Print EOT message
                Break
            Print statistics
            Close connection
        This is where out.txt is created.
    Step 8: Testing and cleaning up
        Test to make sure everything works
        Make sure we have all of the right files
        Make sure we make a detailed README outlined by "Submission Guidlines"
        Make sure it runs on go and the course system
        Zip up all the files and submit

=======================================================

Required includes we need to do this:
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>

    These are built in and the system just does it.
    It gives us access to socket(), bind(), listen(), accept(), connect(), send(), recv()
    If need more information you can see the man pages about them

=======================================================

This is a minimum "toy" server example:
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(sock, (struct sockaddr*)&addr, sizeof(addr));
    listen(sock, 1);

    int client_sock = accept(sock, NULL, NULL);
Copy this code into AI and ask what is happening on each line
This is just the start of server.c that establishes a TCP connection between the client and server after a client requests a connection

=======================================================

htons() => host to network short
    - Convert a 16‑bit integer from your machine’s byte order → network byte order.
ntohs() => network to host short
    - Convert a 16‑bit integer from network byte order → your machine’s byte order.

This is used for converting from little endian to big endian systems
    Networks use Big endian
    Basically all of the client machines use little endian
    so we need to conver it back and forth between big endian and little endian
        ensures data is preserved and interpreted correctly

=======================================================