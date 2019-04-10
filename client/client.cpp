#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>

#include <fstream>
using namespace std;

#include "../config.h"

#define ERROR( msg ) printf("\033[0;32;31mError: %s\033[m\n", msg)
#define INFO( msg ) printf("\033[0;32;32mInfo: %s\033[m\n", msg)

const char* config_filename="../config.txt";
int sockfd = 0;

void* daemon_recv(void* data){
    char re[MAX_MSG_LEN] = "";
    while(1){
        if( recv(sockfd, re, MAX_MSG_LEN, 0)>0 )
            cout << re << endl;
        else{
            cout << "Connection lost!\n";
            close(sockfd);
            break;
        }
    }
    pthread_exit(NULL);
}
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

    struct addrinfo dns_addr, *res=NULL;
    while( res == NULL ){
        // Clear variables
        memset((void *)&dns_addr, 0, sizeof(dns_addr));  // Clear it
        dns_addr.ai_family = AF_INET;
        dns_addr.ai_socktype = SOCK_STREAM;
        dns_addr.ai_protocol = 0;

        printf("Trying to connect with %s:%s\n", hostname, port_number);
        if(getaddrinfo(hostname, port_number, &dns_addr, &res) == 0 && res != NULL){
            INFO("Server connected!");
            if( connect(sockfd, res->ai_addr, res->ai_addrlen) == 0 ){
                pthread_t daemon;
                pthread_create(&daemon, NULL, daemon_recv, NULL);

                char msg[MAX_MSG_LEN] = "";
                while(1){
                    cin >> msg;
                    send(sockfd, msg, strlen(msg), 0);
                }
                //write(sockfd, msg, strlen(msg));
                //close(sockfd);
                //send(sockfd, msg, strlen(msg), 0);
                pthread_join(daemon, NULL);
            }
        }
        else
            ERROR("Fail to get address");
    }
    freeaddrinfo(res);
    return 0;
}
