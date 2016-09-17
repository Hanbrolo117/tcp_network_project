// Created by Kyle Ferguson on 9/11/16.

// Kyle Ferguson
// CSE 434, Computer Networks
// Semester: Fall 2016
// Project Part: 1
// Time Spent: 8 hours
// Server Program


//ANSWERS TO QUESTIONS A & B:
//Question A Answer:
/*
 * Disadvantages:
 * With respect to concurrency, a problem that one can run into, especially when using fork(), is the
 * process creation overhead of the threads. This is because the programmer is not manually managing the thread creation
 * and management. Even so, this can still when manually implemented. This problem can eventually lead to overloading the machine
 * running the concurrent program. The main disadvantage of a connection oriented service, especially for a small network service
 * much like this project, is the time consumption. TCP goes through all the layers of the network as well as carrying out checksums
 * on the amount of data being sent, thus both bandwith and time are taken up more with a connection oriented service.
 *
 *
 * Advantages:
 * First off, a connection oriented server is often considered a "reliable network".
 * A virtual link is made between the client and server, thus the data is guaranteed to
 * arrive in order, thus is reliable. To add to this, if this service is concurrent, then
 * multiple things can be handled by the server. Although this is not true parallelism, the scheduling
 * of handling different tasks, or in the case of networks, multiple clients, allows for an increase in client
 * processing. Thus no one client can hold up the processing of the other clients.
 * */


//Question B Answer:
/*
 *A good application for a connection oriented server would be something like a chat application where multiple clients can
 * connect to a main application via a server that handles their connections and communications. A great example is something
 * like the Twitch website. This project could quickly adapt to such a chat site, the server would host the website, and each
 * message received by any of the connected clients would be displayed on the website. Even though each client is being handled by
 * and individual process, the displaying of each client message on a website allows the expirence to seem connected to the users as
 * they would see all other messages the server received via the website the server is managing.
 * */

#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <vector>
#include <sstream>
#define BUFFSIZE 256
#define MAXWAIT 5

std::vector<int>* clients;

//A function for displaying an error and exiting the program:
void Die(std::string mess){
    perror(mess.c_str());   //Display Error.
    exit(1);                //Exit program.
}

//The main program
int main(int argc, char* argv[]){

    //Setup
    //----------------------------------------------------------------
    //File Descriptors which store values returned by the socket system call and accept system call:
    int sockfd, newsockfd;

    //Stores the Port Number <p> which the Server listens for and accepts connections on:
    int portnum;

    //Stores the size of the address of the client and is required for the accept() system call:
    socklen_t client_len;

    //Set the buffer size for the socket connection:
    char buffer[BUFFSIZE];

    //Stores the address of the server:
    struct sockaddr_in serv_addr;

    //Stores the address of the client:
    struct sockaddr_in client_addr;
    clients  = new std::vector<int>();
    //----------------------------------------------------------------


    //Verification of valid CommandLine arguments:
    //----------------------------------------------------------------
    if(argc != 2){
        fprintf(stderr,"USAGE: server <Server Port Number>\n");
        exit(1);
    }
    //----------------------------------------------------------------


    //Initial Connection:
    //----------------------------------------------------------------

    //Attempt to connect to socket:
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0){
        Die("Error: failed to open socket.");
    }

    //Set server address buffer
    bzero((char*)&serv_addr, sizeof(serv_addr));

    //Convert Command argument 1 from a string to a number
    try {
        portnum = atoi(argv[1]);
    }catch (int e){
        Die("Error: portNumber argument was not a number.");
    }

    //Setup Server sockaddr_in:
    //Set address family:
    serv_addr.sin_family = AF_INET;

    //Set IP address of the host:
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    //Set the port number of the server by converting it to a network short using the Host to network short system call:
    serv_addr.sin_port = htons(portnum);

    //Attempt to Bind the socket to the server address using the socket descriptor, the address to which the server is bound, and the SIZE of the
    //address to which the server is bound:
    //TODO::Find a fix for the bind error that keeps coming up:
    int isBound = bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if ( isBound < 0){
        Die("Error: failed to bind the socket to the server address.");
    }
    //----------------------------------------------------------------


    //Application logic:
    //Listen to port to connect with any client
    //If connection is good, read first message, if client number received is not in client list, continue process with fork()
    //Else deny further communication and close connection with that client.
    //----------------------------------------------------------------

    //Listen on the port for a connection from a client(s):
    listen(sockfd,MAXWAIT);
    int pid;
    while(1){



        //Client Setup:
        //----------------------------------------------------------------
        //Instantiate Client:
        client_len = sizeof(client_addr);

        //Attempt to connect to client, this system call will block the process until a client attempts a connection with the server:
        newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
        if(newsockfd < 0){
            Die("Error: Failed to accept client.");
        }
        //----------------------------------------------------------------


        //Otherwise accept was successful and connection is established:
        //Attempt to read message from client:
        //----------------------------------------------------------------
        int n = read(newsockfd,buffer,BUFFSIZE);
        if(n < 0){
            Die("Error: Failed to read from socket.");
        }
        //----------------------------------------------------------------


        //Check if Client Number is valid for processing:
        //----------------------------------------------------------------
        int clientNum = atoi(buffer);
        bool isValid = true;
        for(int i =0; i < clients->size(); i++){
            if(clients->at(i) == clientNum){
                isValid = false;
            }
        }
        //----------------------------------------------------------------


        //IF the client is valid for proccessing, add client number to list, and proceed to process via a fork():
        //----------------------------------------------------------------
        if(isValid){
            std::cout << "Client " << clientNum << " has successfully connected." << std::endl;
            //Add Client to List:
            clients->push_back(clientNum);
            //Notify client of success
            std::string passedMsg = "Passed client number check.";
            send(newsockfd,passedMsg.c_str(),BUFFSIZE,0);
        }else{
            //Client is not valid, notify client and disconnect:
            std::string failedMsg = "Failed client number check\nduplicate client number...rejected by server.";
            send(newsockfd,failedMsg.c_str(),BUFFSIZE,0);
            close(newsockfd);
            continue;
        }
        //TODO::Fork client and handle the process until the connection is terminated:
        //----------------------------------------------------------------
        pid = fork();
        if(pid < 0){
            close(newsockfd);
            continue;
        }
        //Handle Child Process:
        else if(pid == 0){
            //Run Client handle Method
            int isConnected = 1;
            int msgCount = 0;
            while(isConnected){
                //Receive message:
                //----------------------------------------------------------------
                if(recv(newsockfd,buffer,BUFFSIZE,0) < 0){
                    //Display which Client message failed to be read:
                    std::stringstream ss;
                    ss << "Error: Failed to read message from Client " << clientNum << ".";
                    std::string err = ss.str();
                    Die(err);
                }
                //If the Client Disconnected, end the process handling loop, and exit function:
                if(strcmp(buffer,"disconnect") == 0){
                    std::cout << "Client " << clientNum << " has disconnected." << std::endl;
                    isConnected = 0;
                }else{
                    msgCount++;
                    std::string msg(buffer);
                    std::cout << "received client " << clientNum << "'s msg[" << msgCount <<"]: " << msg << std::endl;
                }
                //----------------------------------------------------------------


                //Send message confirming message received provided there is still a "connection" or willingness to communicate:
                //----------------------------------------------------------------
                if(isConnected){
                    char msg[BUFFSIZE] = "msg received.";
                    //Attempt to send confirmation message to Client_i:
                    if(send(newsockfd,msg, BUFFSIZE,0) < 0){
                        std::stringstream ss;
                        ss << "Error: Failed to send conformation message to Client " << clientNum << ".";
                        std::string err = ss.str();
                        Die(err);
                    }
                }
                //----------------------------------------------------------------


            }//END OF IS-CONNECTED LOOP

            //End Process:
            exit(1);

        }//END OF FORKED PROCESS CODE BLOCK
        //----------------------------------------------------------------

    }//END LISTEN LOOP
    //----------------------------------------------------------------




    return 0;
}//END OF MAIN


#pragma clang diagnostic pop