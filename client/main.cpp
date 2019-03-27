#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <sys/types.h>
#include <unistd.h>

#include <jsoncpp/json/json.h>
#include <fstream>
using namespace std;

#include "../config.h"

#define ERROR( msg ) printf("\033[0;32;31mError: %s\033[m\n", msg)
#define INFO( msg ) printf("\033[0;32;32mInfo: %s\033[m\n", msg)

const string config_filename="../config.json";

int main(int argc , char *argv[])
{
    Json::Value config;
    std::ifstream config_fs(config_filename, std::ifstream::binary);
    config_fs >> config;
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
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");     // Accept any address
    serv_addr.sin_port = htons(8080);           // Set port

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))==-1 ){
        ERROR("Fail to connect");
        return 0;
    }
    const char* msg = "Hello world!\n";
    send(sockfd, msg, strlen(msg), 0);
    close(sockfd);
    return 0;
}
