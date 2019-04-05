#include "connection.h"
#include <cstdio>
#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>

#include "../config.h"
using namespace std;

void Connection::Connection(){
    // Variable initialization
    sockfd = 0;
    memset((void *)&addr, 0, sizeof(addr));  // Clear it
    memset((void *)buffer, 0, sizeof(buffer));

    addr.ai_family = AF_INET;	            // IPv4
    addr.ai_socktype = SOCK_STREAM;
    addr.ai_protocol = 0;
}
int Connection::get_hostname_from_file(char* filename, char* host, char* port){
    FILE *fp = fopen(filename, "r");
    if(fp == NULL 
            || fgets(host, HOST_MAX_LEN, fp) == NULL 
            || fgets(port, HOST_MAX_LEN, fp) == NULL){
        ERROR("Error in reading from config file.");
        return -1;
    }
    host[strlen(host)-1] = '\0';
    port[strlen(port)-1] = '\0';
    return 0;
}
int Connection::read(){
    memset((void *)buffer, 0, sizeof(buffer));
    return read(send_fd, buffer, BUFFER_SIZE);
}
int Connection::send(char* buf){
    return send(send_fd, buf, strlen(buf), 0);
}
int Connection::close(){
    freeaddrinfo(res);
    close(sockfd);
}
int Connection::sock_init(char* config_filename){
    if( get_hostname_from_file(config_filename)!=0 ){
        ERROR("Fail to read from config file.");
        return -1;
    }
    sockfd = socket(AF_INET , SOCK_STREAM , 0);  // Set protocol to 0 (Allow system to choose)
    if (sockfd == -1){
        ERROR("Fail to create a socket.");
        return -1;
    }
    return 0;
}

Server::Server():Connection(){
    sock_addr.sin_addr.s_addr = INADDR_ANY;     // Accept any address
}
int Server::init(){
    addr.ai_flag = AI_PASSIVE;                  // the returned socket addresses will be suitable for bind(2)ing a socket that will accept(2) connections. The returned socket address will contain the "wildcard address"
    if( getaddrinfo(NULL, port_number, addr, &res)==0 && res!=NULL){
        bind(sockfd, res->ai_addr, res->ai_addrlen);
        return 0;
    }
    return -1;
}
int Server::listen(){
    bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    if(listen(sockfd, MAX_CONNECTION_QUEUE) < 0){
        ERROR("Fail to listen on port");
        return -1;
    }
    send_fd = accept(sockfd, (struct sockaddr *)&serv_addr, &socklen);
    if( send_fd < 0 )
        ERROR("Fail to accept");
    else{
        INFO("Connection established!");
    }
    return 0;
}

Client::Client():Connection(){
    sock_addr.sin_addr.s_addr = INADDR_ANY;     // Accept any address
}
int Client::init(){
    if( getaddrinfo(hostname, port_number, addr, &res)==0 && res!=NULL){
        return 0;
    }
    return -1;
}
int Client::connect(){
    send_fd = sockfd;
    return connect(sockfd, res->ai__addr, res->ai_addrlen);
}

