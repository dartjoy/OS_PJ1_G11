#ifndef _CONNECTION_H
#define _CONNECTION_H

#include <cstdio>
#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>

#include "../config.h"

class Connection{
    protected:
        int sockfd, send_fd;
        char hostname[HOST_MAX_LEN];
        char port_number[HOST_MAX_LEN];
        struct addrinfo addr;
        struct addrinfo *res;

        int sock_init(char* config_filename);
        static int get_hostname_from_file(char* filename, char* host, char* port);
    public:
        char buffer[BUFFER_SIZE];
        Connection();
        int close(); 
        int read();
        int send(char *buf);

};

class Server:Connection{
    private:
        int sock_cnn;
    public:
        Server();
        int init();
        int listen();
};

class Client:Connection{
    private:
    public:
        Client();
        int init();
        int connect();
};
#endif

