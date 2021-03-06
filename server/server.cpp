#include<cstdlib>
#include<cstdio>
#include<string.h>
#include<iostream>

#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<unistd.h>
#include <signal.h>
#include <chrono>
#include <ctime>

#include<pthread.h>

#include <fstream>
using namespace std;

#include "../config.h"

// Print error message with color
#define ERROR( msg ) printf("\033[0;32;31mError: %s\033[m\n", msg)
#define INFO( msg ) printf("\033[0;32;32mInfo: %s\033[m\n", msg)

const char* config_filename="../config.txt";
const char* save_file = "../save.txt";

int sockfd = 0; //main socket
int sock_clients[MAX_CLIENT]; //save state of whether a connection is establish
char username[BUFFER_SIZE][MAX_USERNAME];
struct sockaddr_in serv_addr;
char buffer[BUFFER_SIZE];
char client_message[MAX_MESSAGE]; 
socklen_t socklen;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
ofstream save; //collect messages

void Exit(int sig){ //handle ctrl-c signal
	save.close();
	cout << "\033[A\33[k" << endl; //move cursor and clear
	exit(0);
}

void* daemon_accept_client(void* data){ //thread that handle the request
    int newSocket_index = *((int *)data);
    int newSocket = sock_clients[newSocket_index];
    bool greeting_msg = true;
    bool client_alive = true;

    while(client_alive){
        if( recv(newSocket, client_message, MAX_MESSAGE, 0) > 0 ){
            char *message = (char *)(malloc(sizeof(client_message) + 20));
            memset((void *)message, 0, sizeof(*message));
            
            if(greeting_msg){ //welcome message
                strcpy(username[newSocket_index], client_message);
                strcpy(message, client_message);
                strcat(message, " join the chat.");
                greeting_msg = false;
                cout << message << endl;
            }
            else if( strncmp(client_message, "exit", MAX_MESSAGE) == 0 ){ //leave message
                strcpy(message, username[newSocket_index]);
                strcat(message, " leave the chat.");
                cout << message << endl;
                close(newSocket);
                sock_clients[newSocket_index] = 0;
                client_alive = false;
            }
            else{ //copy message to send to other client
                strcpy(message, username[newSocket_index]);
                strcat(message, ": ");
                strcat(message, client_message);
            } 
            
            pthread_mutex_lock(&lock); //cretical section(only one thread can access buffer and send message to other client at a time)

			std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

            strcpy(buffer, message);
			save << std::ctime(&now) << message << endl; //save message to database

            for(int i = 0; i < MAX_CLIENT; i++){ //broadcast
                if( sock_clients[i] != 0 ){
                    send(sock_clients[i], buffer, strlen(buffer), 0);
                }
            }

            free(message);
            memset((void *)&client_message, 0, sizeof(client_message));
            memset((void *)&buffer, 0, sizeof(buffer));

            pthread_mutex_unlock(&lock);	  
        }
    }	
    pthread_exit(NULL);
}

int main(int argc , char *argv[])
{
	signal(SIGINT, Exit); //signal handler

	/*initialize everything*/
    char hostname[HOST_MAX_LEN];
    char port_number[HOST_MAX_LEN];
    FILE *fp = fopen(config_filename, "r");
    if(fp == NULL 
            || fgets(hostname, HOST_MAX_LEN, fp) == NULL 
            || fgets(port_number, HOST_MAX_LEN, fp) == NULL){
        ERROR("Error in reading from config file.");
        return 0;
    }
    hostname[strlen(hostname)-1] = '\0';
    port_number[strlen(port_number)-1] = '\0';

    /* Socket connection build up */
    for(int i = 0; i < MAX_CLIENT; ++i) sock_clients[i] = 0;
    // AF_INET:  Transfer data between different computer (IPv4)
    // AF_INET6: With IPv6
    // AF_UNIX/AF_LOCAL: Transfer between process(In the same computer)
    // SOCK_STREAM: (TCP)
    // SOCK_DGRAM:  (UDP)
    sockfd = socket(AF_INET , SOCK_STREAM , 0);  // Set protocol to 0 (Allow system to choose)
    if (sockfd == -1){
        ERROR("Fail to create a socket.");
        return 0;
    }

    /* Declare and initialize */
    memset((void *)&serv_addr, 0, sizeof(serv_addr));  // Clear it

    serv_addr.sin_family = AF_INET;	            // IPv4
    serv_addr.sin_addr.s_addr = INADDR_ANY;     // Accept any address
    serv_addr.sin_port = htons(atoi(port_number));           // Set port

    bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if(listen(sockfd, MAX_CONNECTION_QUEUE) < 0){
        ERROR("Fail to listen on port");
        return 0;
    }
    INFO("Listening...");

    int i = 0;
    pthread_t daemon_thread[MAX_CLIENT];

	save.open(save_file);

    while(1){ //main section to accept connection
        int sockclit = accept(sockfd, (struct sockaddr *)&serv_addr, &socklen);
        int sock_index;
        for(int i = 0; i < MAX_CLIENT; i++){
            if( sock_clients[i] == 0 ){
                sock_clients[i] = sockclit;
                sock_index = i;
                break;        
            }
        }
        if(sockclit > 0) INFO("Get New Connection");
		/*create thread to handle client's request*/
        if(pthread_create(&daemon_thread[i++], NULL, daemon_accept_client, &sock_index) != 0)
            INFO("Failed to create thread");
    }
    return 0;
}
