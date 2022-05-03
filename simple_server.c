#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <signal.h>

#define PACKET_SIZE 1024
#define PORT 8080

int listenfd = -1;
int acceptfd = -1;

void signal_handler(int signum) {
    printf("signal_handler: caught signal %d\n", signum);
    if (signum == SIGINT) {
        printf("SIGINT\n");
        
        if(listenfd != -1)
            close(listenfd);
        
        if(acceptfd != -1)
            close(acceptfd);

        exit(1);
    }
}

int main(int argc, char *argv[])
{
    int ret;
    struct sockaddr_in serv_addr; 
    char buffer[PACKET_SIZE];
    //time_t ticks; 
    
    signal(SIGINT, signal_handler);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(buffer, '\0', sizeof(buffer)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT); 
    
    ret = bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
    if(ret < 0) {
        printf("bind error\n");
    }
   
    ret = listen(listenfd, 10); 
    if(ret < 0) {
        printf("listen error\n");
    }

    while(1)
    {
        printf("server listening ...\n");
        acceptfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
        if(acceptfd != -1) {
            printf("client connected\n");
            ret = read(acceptfd, buffer, PACKET_SIZE);
            printf("recieved %s from client %s bytes\n", buffer, ret);
            close(acceptfd);
            break;    
        }
        //ticks = time(NULL);
        //snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
        //write(connfd, sendBuff, strlen(sendBuff)); 
        //close(connfd);
        //sleep(1);
     }
}
