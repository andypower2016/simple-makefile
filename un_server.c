/* Server program using AF_UNIX address family */
/* Headers */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <time.h>

#include "cmd.h"

#define SERVER_PATH     "/tmp/server"
#define BUFFER_LENGTH    1024
#define FALSE            0

fd_set master; 
fd_set read_fds; 
int fdmax; 
int i;

void CloseServer()
{
   for(i = 0 ; i <= fdmax ; ++i) {
      if(FD_ISSET(i, &master)) {
         close(i);
      }
   }
   unlink(SERVER_PATH);
}

void sighandler(int sig)
{
   printf("Closed Server\n");
   CloseServer();
}



void HandleResponse(int fd, RxBuffer* rxBuffer)
{
   char buffer[BUFFER_LENGTH];
   int rc;
   rc = recv(fd, buffer, BUFFER_LENGTH, 0);
   cmd* recvCmd = 0;
   
   if(rc <= 0) {
     printf("[%s] timeout\n", __FUNCTION__);
   }
   else {
     recvCmd = (cmd*) buffer;
     switch(recvCmd->cmdID)
     {
      case CMD_ID_SYSTIME:
           {
		printf("[%s] recv system time=%u\n",__FUNCTION__,*(uint32_t*)recvCmd->buffer);
           }
           break;
      default:
           printf("recv cmd not recognized\n");
           break;
     }	
   }
}

void func1(int fd)
{
   char buffer[BUFFER_LENGTH];
   int rc;
   int count = 3;
   
   while(count--)
   {
      RxBuffer rxBuffer = {0};    
      SendCommand(fd, CMD_ID_GETSYSTIME, 0,0); 
      HandleResponse(fd, &rxBuffer); 
   }
}


int HandleCommand(int fd, char buffer[])
{
   cmd *recvCmd = NULL;
   recvCmd = (cmd*) buffer;
   printf("[%s] Handle cmd %d \n", __FUNCTION__, recvCmd->cmdID);

   switch(recvCmd->cmdID)
   {
      case CMD_ID_FUNC1:
           {
	      func1(fd);	
           }
           break;
      default:
           printf("recv cmd not recognized\n");
           break;
   }	
   return 0;
}

void Recieve(int fd) 
{
   char buffer[BUFFER_LENGTH];
   int rc;
   
   memset(buffer, 0, BUFFER_LENGTH);
   rc = recv(fd, buffer, BUFFER_LENGTH, 0);
   if(rc == 0) {
      /* client close connection */
      printf("Client[%d] closed connection\n", fd);
      close(fd);
      FD_CLR(fd, &master); 

   } else if (rc > 0) {
   
      printf("Server recv %d bytes from client[%d]\n", rc, fd);    
      HandleCommand(fd, buffer);
      SendCommand(fd, CMD_ID_END, 0, 0);    
   }
   else if(rc < 0){
      printf("[%s] recv timeout\n", __FUNCTION__);
   }
}

int main()
{

   int    listenfd=-1, acceptfd=-1;
   int    rc;
   char   buffer[BUFFER_LENGTH];
   struct sockaddr_un serveraddr;

   signal(SIGINT, sighandler);

   do
   {
      listenfd = socket(AF_UNIX, SOCK_STREAM, 0);
      if (listenfd < 0)
      {
         perror("socket() failed");
         break;
      }

      memset(&serveraddr, 0, sizeof(serveraddr));
      serveraddr.sun_family = AF_UNIX;
      strcpy(serveraddr.sun_path, SERVER_PATH);

      unlink(SERVER_PATH);
      rc = bind(listenfd, (struct sockaddr *)&serveraddr, SUN_LEN(&serveraddr));
      if (rc < 0)
      {
         perror("bind() failed");
         break;
      }

      rc = listen(listenfd, 10);
      if (rc< 0)
      {
         perror("listen() failed");
         break;
      }

   } while (FALSE);

   printf("Server ready for client to connect()...\n");

   FD_ZERO(&master); 
   FD_ZERO(&read_fds);

   FD_SET(listenfd, &master);
   fdmax = listenfd; 
   
   /* main loop */
   while(1) {

      read_fds = master;

      printf("server select ...\n");
      rc = select(fdmax+1, &read_fds, NULL, NULL, NULL);

      if(rc == -1) {
         printf("server select error or terminate ...\n");
         //CloseServer();
         return 0;
      }
      
      for(i = 0 ; i <= fdmax ; ++i) 
      {
         if(FD_ISSET(i, &read_fds)) 
         {
            if(i == listenfd) 
            {
               acceptfd = accept(i, NULL, NULL);
               if(acceptfd == -1) 
               {
                  printf("accept error\n");
               }
               else 
               {
                  FD_SET(acceptfd, &master);
                  fdmax = acceptfd > fdmax ? acceptfd : fdmax;
                  printf("New connection from client[%d]\n", acceptfd);

                  struct timeval tv;
                  tv.tv_sec = 5;
                  tv.tv_usec = 0;
                  setsockopt(acceptfd, SOL_SOCKET, SO_SNDTIMEO, (const char*) &tv, sizeof(tv));   
                  setsockopt(acceptfd, SOL_SOCKET, SO_RCVTIMEO, (const char*) &tv, sizeof(tv));   
               }
            }  
            else /* handle connected client */
            {
               Recieve(i);
            }          
         }
      }
   }
   
   CloseServer();

   return 0;
}
