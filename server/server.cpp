#include<cstdlib>
#include<cstdio>
#include<string.h>
#include<iostream>

#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<unistd.h>

#include <fstream>
using namespace std;

#include "../config.h"

// Print error message with color
#define ERROR( msg ) printf("\033[0;32;31mError: %s\033[m\n", msg)
#define INFO( msg ) printf("\033[0;32;32mInfo: %s\033[m\n", msg)

const char* config_filename="../config.txt";

int main(int argc , char *argv[])
{
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
    int sockfd = 0;
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
	struct sockaddr_in serv_addr;
    socklen_t socklen;
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
    

    char buffer[BUFFER_SIZE];
    while(1){
        int sock_cnn = accept(sockfd, (struct sockaddr *)&serv_addr, &socklen);
        if( sock_cnn < 0 )
            ERROR("Fail to accept");
        else{
            INFO("Connection established!");
            
            // Clear old data
            memset((void *)buffer, 0, sizeof(buffer));
            int err = read(sock_cnn, buffer, BUFFER_SIZE);
            printf("Get message: %s", buffer);
        }
    }
    return 0;
}