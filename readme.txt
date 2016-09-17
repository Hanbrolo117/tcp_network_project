Project 1 by Kyle Ferguson

To COMPILE:

1. On your server machine simply compile like so:
        g++ server.cpp -o server

2. On your client(s) machine(s) simply compile like so:
        g++ client.cpp -o client

To USE:

1. run server first -> $ ./server <portNumber>
2. run client(s)    -> $ ./client <host dotted name> <client's "id number"> <portNumber>

Note:   - "portNumber" for server and client(s) should be the same
        - the client id can be any number, it is just a user/client defined parameter for the server to track who is who.

Example:
    On Server:
        $ ./server 50050

    On some Client:
        $ ./client 127.0.0.0 4 50050



Application:
    - Once a client successfully connects, the client can send messages to the server, the server will simply reply, verifying
    the message was received. If the Client uses and Id that the server has/currently served, the client will be denied. Client(s)
    who are denied will be notified that the client number used has failed the client number check and were thus denied.

    - The Server will log to console when a "client i" has successfully connected or disconnected. The server will print each client(s)
     message to the console in the following format:
        "[Client i] : msg[ Client i's jth message] : [Client i's message that was received] "

    - The Server handles each client concurrently and uses tcp protocols.