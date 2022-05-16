/* Client program using AF_UNIX address family */
/* Headers */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>

#include "cmd.h"

#define SERVER_PATH     "/tmp/server"
#define BUFFER_LENGTH    1024
#define FALSE              0

int fd = -1;
pthread_t handleRecvThread;
int g_end = 0;

void CloseSocket()
{
   if (fd != -1)
      close(fd);
}

void sighandler(int sig)
{
   CloseSocket();
   pthread_join(handleRecvThread, 0);
   g_end = 1;
   exit(1);
}

int HandleCommand(int fd, char buffer[])
{
   cmd* recvCmd = (cmd*) buffer;
   int sendlen = 0;
   uint8_t *data = 0;
   int rc = 0;
   
   printf("[%s] Handle cmd %d \n", __FUNCTION__, recvCmd->cmdID);
   switch(recvCmd->cmdID)
   {
      case CMD_ID_GETSYSTIME:
           {
                uint32_t sysTime = 102;
                sendlen = sizeof(cmd) + sizeof(uint32_t);
                SendCommand(fd, CMD_ID_SYSTIME, (char*)&sysTime, sizeof(uint32_t));
           }
      case CMD_ID_END:
           
           return 1;
      default:
           printf("recv cmd not recognized\n");
           return 0;
   }
   return 0;
}

int Recieve(int fd) {

   char buffer[BUFFER_LENGTH];
   cmd* recvCmd;
   int bEnd = 0;
   int rc = 0;
   
   while(!bEnd) {

      memset(buffer, 0, BUFFER_LENGTH);
      rc = recv(fd, buffer, BUFFER_LENGTH, 0);
      if(rc <= 0) {
         
         printf("[%s] recv nothing or timeout [rc=%d]\n", __FUNCTION__, rc);
         bEnd = 1;

      } 
      else if (rc > 0) {

         bEnd = HandleCommand(fd, buffer);         
      }
   }
   return rc;
}


void OnRecieve(void *param)
{
   int socket = *(int*) param;
   int fdmax;
   int rc;
   printf("[%s] start, socket=%d\n",__FUNCTION__, socket);
   
   fd_set readfds;
   FD_ZERO(&readfds);
   FD_SET(socket, &readfds);
   fdmax = socket;
   while(!g_end) {

      printf("[%s] select\n",__FUNCTION__);
      rc = select(fdmax+1, &readfds, NULL, NULL, NULL);
      if(FD_ISSET(socket, &readfds))
      {
         if(rc > 0) {
            printf("[%s] socket recv\n",__FUNCTION__);
            rc = Recieve(socket);
            if(rc <= 0) {
               /* server close connection */
      	      printf("Server[%d] closed connection\n", fd);
      	      close(fd);
      	      FD_CLR(fd, &readfds); 
      	      g_end = 1;
            }
         }
      }

   }
}

int main(int argc, char *argv[])
{

   int    rc;
   char   buffer[BUFFER_LENGTH];
   int sendlen;
   struct sockaddr_un serveraddr;
   signal(SIGINT, sighandler);
  
   fd = socket(AF_UNIX, SOCK_STREAM, 0);
   if (fd < 0)
   {
      perror("socket() failed");
      return 0;
   }

   memset(&serveraddr, 0, sizeof(serveraddr));
   serveraddr.sun_family = AF_UNIX;
   if (argc > 1)
      strcpy(serveraddr.sun_path, argv[1]);
   else
      strcpy(serveraddr.sun_path, SERVER_PATH);


   rc = connect(fd, (struct sockaddr *)&serveraddr, SUN_LEN(&serveraddr));
   if (rc < 0)
   {
      perror("connect() failed");
      close(fd);
      return 0;
   }
   
   printf("fd=%d\n",fd);

   struct timeval tv;
   tv.tv_sec = 3;
   tv.tv_usec = 0;
   setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char*) &tv, sizeof(tv));   
   setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*) &tv, sizeof(tv));

   // create pthread handle recv
   pthread_create(&handleRecvThread, 0, OnRecieve, (void*) &fd);

   while(1) {
      
      /* send cmd to server */
      printf("Input cmd ...\n");
      memset(buffer,0,BUFFER_LENGTH);
      fgets(buffer, sizeof(buffer), stdin);

      cmd sendCmd={0};
      sendCmd.cmdID = atoi(buffer);
      memcpy(buffer, (char*)&sendCmd, sizeof(cmd));
      sendlen = sizeof(cmd);
      rc = send(fd, buffer, sendlen, 0);
      if(rc > 0) {         
         //printf("send %s to server success\n", buffer);
      }
   }

   CloseSocket();
   pthread_join(handleRecvThread, 0);

   return 0;
}
