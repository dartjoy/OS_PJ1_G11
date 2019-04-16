#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <iostream>
#include <string>
#include <sstream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>

#include <fstream>
using namespace std;

#include "../config.h"

#define ERROR( msg ) printf("\033[0;32;31mError: %s\033[m\n", msg)
#define INFO( msg ) printf("\033[0;32;32mInfo: %s\033[m\n", msg)
#define MSG( msg ) printf("\x1b[36m%s\033[m\n", msg)

const char* config_filename="../config.txt";
int sockfd = 0;

void Exit(int sig){
    char msg[MAX_MESSAGE] = "exit";
    send(sockfd, msg, strlen(msg), 0);
    cout << "\rGoodbye!" << endl;
    close(sockfd);
    exit(0);
}

void* daemon_recv(void* data){ //keep receiving message from server
    char re[MAX_MESSAGE] = "";
    while(1){
        if( recv(sockfd, re, MAX_MESSAGE, 0) > 0 ){
            MSG(re);
            memset((void*)re, 0, sizeof(re));
        }
        else{ //connection terminate
    		pthread_exit(NULL);
        }
    }
}

int main(int argc , char *argv[])
{
    signal(SIGINT, Exit);

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

    char username[MAX_USERNAME] = "";
    INFO("Please enter username");
    cin >> username;
    struct addrinfo dns_addr, *res = NULL;
    while( res == NULL ){
        // Clear variables
        memset((void *)&dns_addr, 0, sizeof(dns_addr));  // Clear it
        dns_addr.ai_family = AF_INET;
        dns_addr.ai_socktype = SOCK_STREAM;
        dns_addr.ai_protocol = 0;

        printf("Trying to connect with %s:%s\n", hostname, port_number);
        if(getaddrinfo(hostname, port_number, &dns_addr, &res) == 0 && res != NULL){
            INFO("Server connected!\n");
            if( connect(sockfd, res->ai_addr, res->ai_addrlen) == 0 ){
                send(sockfd, username, strlen(username), 0);
                pthread_t daemon;
                pthread_create(&daemon, NULL, daemon_recv, NULL);

                char msg[MAX_MESSAGE] = "";
                while(1){
                    memset((void*)msg, 0, sizeof(msg));
                    cin.getline(msg, MAX_MESSAGE);
                    if( strncmp(msg, "exit", MAX_MESSAGE) == 0 ){ //client terminate condition
                    	cout << "\033[A\33[k";
						Exit(0);
                        break;
					}
                    cout << "\033[A\33[2k"; // Move cursor to previous line and clear it
                    send(sockfd, msg, strlen(msg), 0);
                }
            }
            else
                ERROR("Fail to get address");
        }
    }
    freeaddrinfo(res);
    Exit(0);
    return 0;
}
