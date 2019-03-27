#include<cstdlib>
#include<cstdio>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>

using namespace std;

// Print error message with color
#define ERROR( msg ) printf("\033[0;32;31mError: %s\033[m\n", msg)
#define INFO( msg ) printf("\033[0;32;32mInfo: %s\033[m\n", msg)

const char* config="../config.json";

int main(int argc , char *argv[])
{
    int sockfd = 0;
    // AF_INET:  Transfer data between different computer (IPv4)
    // AF_INET6: With IPv6
    // AF_UNIX/AF_LOCAL: Transfer between process(In the same computer)
    sockfd = socket(AF_INET , SOCK_STREAM , 0);  // Set protocol to 0 (Allow system to choose)
    if (sockfd == -1){
        ERROR("Fail to create a socket.");
        return 0;
    }
    INFO("Listening...");
    
	struct sockaddr_in serv_addr;
    memset((void *)&serv_addr, 0, sizeof(serv_addr));  // Clear it
    serv_addr.sin_family = AF_INET;	            // IPv4
    serv_addr.sin_addr.s_addr = INADDR_ANY;     // Accept any address
    serv_addr.sin_port = htons(8080);           // Set port
    
    return 0;
}
