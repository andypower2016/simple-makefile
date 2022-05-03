#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/un.h>

//#include <arpa/inet.h>
//#include "sock.h"

#define PACKET_SIZE 1024
#define PortNumber 8080

void report(const char* msg, int terminate) {
  perror(msg);
  if (terminate) exit(-1); /* failure */
}

void messageHandler(char* msg, int fd);
void callfunc(int fd);

int main() 
{
  int fd = socket(AF_UNIX,     /* network versus AF_LOCAL */
                  SOCK_STREAM, /* reliable, bidirectional, arbitrary payload size */
                  0);          /* system picks underlying protocol (TCP) */

  if (fd < 0) report("socket", 1); /* terminate */

  /* bind the server's local address in memory */
  struct sockaddr_in saddr;
  memset(&saddr, 0, sizeof(saddr));          /* clear the bytes */
  saddr.sin_family = AF_INET;                /* versus AF_LOCAL */
  saddr.sin_addr.s_addr = htonl(INADDR_ANY); /* host-to-network endian */
  saddr.sin_port = htons(PortNumber);        /* for listening */

  /* bind to socket */
  if (bind(fd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0)
    report("bind", 1); /* terminate */

  /* listen to the socket */
  if (listen(fd, MaxConnects) < 0) /* listen for clients, up to MaxConnects */
    report("listen", 1); /* terminate */

  fprintf(stderr, "Listening on port %i for clients...\n", PortNumber);


  /* a server traditionally listens indefinitely */
  struct sockaddr_in caddr; /* client address */
  int len = sizeof(caddr);  /* address length could change */
  int client_fd = accept(fd, (struct sockaddr*) &caddr, &len);  /* accept blocks */
  if (client_fd < 0) {
    report("accept", 0); /* don't terminate, though there's a problem */
    //continue;
  }

  /* Handle Client infinite */
  while (1) {

    printf("Waiting Client Message ...\n");

    /* read from client */
    char buffer[PACKET_SIZE + 1];
    memset(buffer, '\0', sizeof(buffer));
    int readBytes = read(client_fd, buffer, sizeof(buffer));
    if (readBytes > 0) {
      //puts(buffer);
      //write(client_fd, buffer, sizeof(buffer)); /* echo as confirmation */
      printf("recieved message %s from client\n", buffer);

      /* Parse message and handle it */
      messageHandler(buffer, client_fd);
    }
    
  }  /* while(1) */
  close(client_fd); /* break connection */
  return 0;
}

void messageHandler(char* msg, int fd)
{
  int flag = 1;
  while(flag) {
      
    if(strcmp(msg, "callfunc")==0) {
      callfunc(client_fd);
    }
    else if(strcmp(msg, "end")==0) {  /* end message handler */
      flag = 0;
    }
  }
}

void callfunc(int fd) {
    
  /* get systime */
  char buffer[PACKET_SIZE + 1];
  memset(buffer, '\0', sizeof(buffer));
  memcpy(buffer,"reqsystime",10);
  write(fd, buffer, sizeof(buffer));
  
  memset(buffer, '\0', sizeof(buffer));
  int readBytes = read(fd, buffer, sizeof(buffer));
  if (readBytes > 0) {
    //puts(buffer);
    //write(client_fd, buffer, sizeof(buffer)); /* echo as confirmation */
    printf("recieved systime %s from client\n", buffer);
  }
}