#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>


#define PACKET_SIZE 1024
#define PORT 8080

int fd = -1;

void signal_handler(int signum) 
{
    printf("signal_handler: caught signal %d\n", signum);
    if (signum == SIGINT) {
        printf("SIGINT\n");
        
        if(sock != -1)
            close(sock);
        
        exit(1);
    }
}


// Returns hostname for the local computer
void checkHostName(int hostname)
{
    if (hostname == -1)
    {
        perror("gethostname");
        exit(1);
    }
}
  
// Returns host information corresponding to host name
void checkHostEntry(struct hostent * hostentry)
{
    if (hostentry == NULL)
    {
        perror("gethostbyname");
        exit(1);
    }
}
  
// Converts space-delimited IPv4 addresses
// to dotted-decimal format
void checkIPbuffer(char *IPbuffer)
{
    if (NULL == IPbuffer)
    {
        perror("inet_ntoa");
        exit(1);
    }
}


char* getlocalhostip()
{
    char hostbuffer[256];
    char *IPbuffer;
    struct hostent *host_entry;
    int hostname;
  
    // To retrieve hostname
    hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    checkHostName(hostname);
  
    // To retrieve host information
    host_entry = gethostbyname(hostbuffer);
    checkHostEntry(host_entry);
  
    // To convert an Internet network
    // address into ASCII string
    IPbuffer = inet_ntoa(*((struct in_addr*)
                           host_entry->h_addr_list[0]));
    return IPbuffer;
}
 
int main(int argc, char const* argv[])
{
    int bytes = 0;
    struct sockaddr_in serv_addr;
    char* hello = "Hello from client";
    char* localip = NULL;
    char buffer[PACKET_SIZE] = {0};
    int ret ;
    
    signal(SIGINT, signal_handler);
    
    fd = socket(AF_INET, SOCK_STREAM, 0)
    if (fd < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    localip = getlocalhostip();
    printf("local host ip = %s", localip);
 
    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    ret = inet_pton(AF_INET, /*"127.0.0.1"*/ localip , &serv_addr.sin_addr);
    if (ret <= 0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    
    ret = connect(fd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));
    if (ret < 0) 
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    bytes = send(fd, hello, strlen(hello), 0);
    printf("Hello message sent %s \n", bytes);
    //valread = read(sock, buffer, 1024);
    //printf("%s\n", buffer);

    close(fd);
    return 0;
}
