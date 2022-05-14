/**************************************************************************/
/* This sample program provides code for a client application that uses     */
/* AF_UNIX address family                                                 */
/**************************************************************************/
/**************************************************************************/
/* Header files needed for this sample program                            */
/**************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <time.h>
/**************************************************************************/
/* Constants used by this program                                         */
/**************************************************************************/
#define SERVER_PATH     "/tmp/server"
#define BUFFER_LENGTH    1024
#define FALSE              0

int fd = -1;

void CloseSocket()
{
   if (fd != -1)
      close(fd);
}

void sighandler(int sig)
{
   CloseSocket();
   exit(1);
}


int HandleMessage(int fd, char message[])
{
   char buffer[BUFFER_LENGTH];
   int rc;
   if(strcmp("getsystemtime", message) == 0)
   {
      strcpy(buffer,"100");
      buffer[3] = '\0';
      rc = send(fd, buffer, BUFFER_LENGTH, 0);
      if(rc > 0) {
         printf("[%s] send message %s to server\n", __FUNCTION__, buffer);
      }

      /*memset(buffer,0,sizeof(buffer));
      rc = recv(fd, buffer,sizeof(buffer), 0);
      buffer[rc] = '\0';
      if(rc > 0 && (strcmp(buffer, "ack") == 0 ))
      {
         printf("[%s] recv ack from server\n",__FUNCTION__);
      }*/
      return 0;
   }
   else if(strcmp("end", message) == 0)
   {

      return 1; /* end */
   }
   else
   {
      printf("[%s] not recognized message\n",__FUNCTION__);
      return 1; 
   }
}

void Recieve(int fd) {

   char buffer[BUFFER_LENGTH];
   int bEnd = 0;

   while(!bEnd) {

      memset(buffer, 0, BUFFER_LENGTH);
      int rc = recv(fd, buffer, BUFFER_LENGTH, 0);
      if(rc <= 0) {
         
         printf("[%s] recv nothing or timeout [rc=%d]\n", __FUNCTION__, rc);
         bEnd = 1;

      } else if (rc > 0) {

         buffer[rc] = '\0';
         printf("recv %s from server[%d]\n", buffer, fd);        
         bEnd = HandleMessage(fd, buffer);
         if(bEnd == 1) {
            printf("[%s] Server send end\n", __FUNCTION__);
         }
      }
   }
   
}

int main(int argc, char *argv[])
{

   int    rc;
   char   buffer[BUFFER_LENGTH];
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

   struct timeval tv;
   tv.tv_sec = 3;
   tv.tv_usec = 0;
   setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char*) &tv, sizeof(tv));   
   setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*) &tv, sizeof(tv));

   while(1) {
      
      /* send data to server */
      /*memset(buffer,0,BUFFER_LENGTH);
      strcpy(buffer,"Hello Server");*/
      printf("Input message ...\n");
      memset(buffer,0,BUFFER_LENGTH);
      fgets(buffer, sizeof(buffer), stdin);
      int sendlen = strlen(buffer);
      //fflush(fd);
      rc = send(fd, buffer, sendlen, 0);
      if(rc > 0) {
         buffer[rc-1] = '\0';
         printf("send %s to server success\n", buffer);
         Recieve(fd);
      }

      /*
      printf("Wait for ack from server ...\n");
      memset(buffer,0,BUFFER_LENGTH);
      rc = recv(fd, buffer, BUFFER_LENGTH, 0);
      if(rc > 0) {
         printf("Data from server : %s\n", buffer);
      }*/
      while(recv(fd, buffer, BUFFER_LENGTH, 0)>=0) {
        
      }
   }

   CloseSocket();

   return 0;
}
