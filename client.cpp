// Created by Kyle Ferguson on 9/11/16.

// Kyle Ferguson
// CSE 434, Computer Networks
// Semester: Fall 2016
// Project Part: 1
// Time Spent: 8 hours
// Client Program

#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <c++/v1/iostream>

using namespace std;

#define BUFFSIZE 256
void Die(string mess){
    perror(mess.c_str());
    exit(0);
}


//Function to print the response from the server:
void serverResponse(std::string rspn){
    std::cout << "recv from server: " << rspn << std::endl;
}

//The main program
int main(int argc, char* argv[]){
    //Setup
    //----------------------------------------------------------------
    int sock;
    struct sockaddr_in server;
    char buffer[BUFFSIZE];
    int received = 0;
    char* h;
    char* c;
    char* p;
    //----------------------------------------------------------------


    //Verify Valid Commands:
    //----------------------------------------------------------------
    if(argc != 4){
        fprintf(stderr,"USAGE: client <hostname> <clientNumber> <portNumber>\n");
        exit(1);
    }
    //----------------------------------------------------------------


    //Get commandline arguments:
    //----------------------------------------------------------------
    h = argv[1];
    c = argv[2];
    p = argv[3];
    //----------------------------------------------------------------


    //Create a TCP socket:
    //----------------------------------------------------------------
    sock = socket(PF_INET, SOCK_STREAM,IPPROTO_TCP);
    if(sock < 0){
        //socket() returned -1, thus kill program:
        Die("Error: Failed to created socket.");
    }
    //----------------------------------------------------------------


    //Construct the socket address structure for the server:
    //----------------------------------------------------------------
    //Clear Struct:
    memset(&server, 0, sizeof(server));

    //Set the Internet Protocol for the connection structure:
    server.sin_family = AF_INET;

    //Set the Internet Protocol (IP) address to the hostName <h> provided:
    if(inet_addr(h) < 0){
        Die("Failed to convert host name to IPv4 dotted decimal notation.");
    }
    server.sin_addr.s_addr = inet_addr(h);

    //Set the server port of the server to the portNumber <p> provided:
    server.sin_port = htons(atoi(p));

    //Finally attempt to establish connection with the server:

    if( connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0){
        //connect() failed and returned -1, thus kill program and notify why:
        Die("Error: Failed to connect with Server");
    }
    //----------------------------------------------------------------
    //Otherwise the socket is now ready to accept I/O of data:


    // Attempt to send the Client Number to the Server program, whilst verifying it
    // against the size of the message:
    //----------------------------------------------------------------
    if(send(sock,c,BUFFSIZE,0) < 0){
        Die("Error: message failed to send to server.");
    }
    //----------------------------------------------------------------
    //Otherwise Client Number was sent successfully.


    //Read Server Response. If the message is "Denied", Client could not connect:
    //----------------------------------------------------------------
    recv(sock, buffer, BUFFSIZE, 0);
    if(strcmp(buffer,"Failed client number check\n"
            "duplicate client number...rejected by server.") == 0){
        serverResponse(buffer);
        exit(0);
    }
    //----------------------------------------------------------------
    //Otherwise Server has accepted the handshake with the Client and communications can continue:
    int isConnected = 1;
    serverResponse(buffer);


    //Process any additional requests from user:
    //----------------------------------------------------------------
    printf("Enter: \"\\x\" to disconnect with server.\n\n");
    while(isConnected == 1){
        //Prompt User and get input:
        //----------------------------------------------------------------
        printf("Enter message: ");
        std::string message = "";
        std::getline(std::cin,message);
        //----------------------------------------------------------------


        //Handle Input from user:
        //----------------------------------------------------------------
        if(message.compare("\\x") == 0){
            //Terminate Communication loop:
            isConnected = 0;
            //Notify Server of disconnect:
            char msg[BUFFSIZE] = "disconnect";
            if(send(sock,msg, BUFFSIZE,0) < 0){
                Die("Error: Disconnect message failed to send to server.");
            }
        }else{
            //Otherwise continue communications with server:
            char usrMsg[BUFFSIZE];
            strcpy(usrMsg,message.c_str());
            if(send(sock,usrMsg,BUFFSIZE,0) < 0){
               Die("Error: message failed to send to server.");
            }


        }
        //----------------------------------------------------------------

        if(isConnected){//Finally, if still connected (application logic wise):
            //Get response from Server:
            if(recv(sock,buffer, BUFFSIZE,0) < 0){
                Die("Error: Failed to read confirmation message from server.");
            }
            serverResponse(buffer);
        }
    }//END OF WHILE LOOP
    //----------------------------------------------------------------
    //While-loop has been terminated.


    //Properly close out socket connection:
    //----------------------------------------------------------------
    printf("Connection Terminated...\n");
    close(sock);
    //----------------------------------------------------------------
    return 0;
}//END OF MAIN

