Devon Weckherlen and Achilles Demerutis

This will just be a note doc until we make the actual readme

What we actually need to do:
    To run the program you open 2 terminals: one for the client and one for the server
        Ex: ./server to start the server and then ./client to start the client after compiling

    The server will "have possession" of a file that the client wants to recieve. This file will be called test.txt or something
    The client is requesting the test.txt file from the server and it will be transfered over TCP as a series of packets
    The client writes the data from these packets into a file called out.txt
    
    Server:
        - Creates a listening socket
        - Waits for a client
        - Prints nothing and just waits until there is a client

    Client:
        - Asks for something like: "Enter Filename: "
        - 
