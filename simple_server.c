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

#define PACKET_SIZE 1024
#define PORT 8080

int main(int argc, char *argv[])
{
    int listenfd = 0;
    int acceptfd = 0;
    struct sockaddr_in serv_addr; 
    int ret;

    char buffer[PACKET_SIZE];
    //time_t ticks; 

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

    while(1)
    {
        acceptfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
        if(acceptfd != -1) {
            printf("client connected\n");
            read(acceptfd, buffer, PACKET_SIZE);
            printf("recieved %s from client\n", buffer);
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