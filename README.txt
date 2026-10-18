Relevant Files:
    - packet.h: Header file for packet.c. Included in both client.c and server.c
    - packet.c: Functions for send_all(), recv_exact(), send_packet(), and recv_packet() helper Functions
    - server.c: All logic for running the server side that will create a listening port, connect a client, and send packets
    - client.c: All logic for running the client side where the user will request a filename and then be transmitted that file over a series of packets
    - Makefile: Used to make server and client executable

Compilation Instructiuons:
    - In the terminal do "make"
        - This will create the server and client executable

Configuration Information: 
    - The server and client use hard‑coded configuration values for the hostname and port number.
      There are no external configuration files required for this project.
    - server.c: at the top of the file there is a:
        - #define PORT 9000, which is the servers listening port
    - client.c: at the top of the file there is a:
        - #define PORT 9000, which is the servers listening port that we want to connect to
        - #define SERVER_IP "127.0.0.1", which is the IP address of the machine the client connects to
            - Note: SERVER_IP defaults to "127.0.0.1" for local testing. 
              If testing across multiple machines on the same LAN, this value must be changed 
              to the server machine’s private LAN IP address
        - #define OUTPUT_FILE "out.txt", which is the name of the file we write out to

Running Instructiuons:
    - In terminal 1 (the server) do "./server". This will start the server
    - In terminal 2 (the client) do "./client". This will start the client and ask for the name of a file to be requested

Group Information: 
    Devon Weckherlen - ID: 702757732
        - Contribution: server.c, packet.c, packet.h, Makefile, README
    Achilles Demerutis - ID: 702753287
        - Contribution: client.c, packet.c, README 