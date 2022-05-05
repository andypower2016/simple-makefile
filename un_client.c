/**************************************************************************/
/* This sample program provides code for a client application that uses     */
/* AF_UNIX address family                                                 */
/**************************************************************************/
/**************************************************************************/
/* Header files needed for this sample program                            */
/**************************************************************************/
#include <stdio.h>
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


   while(1) {
      
      /* send data to server */
      memset(buffer,0,BUFFER_LENGTH);
      strcpy(buffer,"Hello Server");
      rc = send(fd, buffer, BUFFER_LENGTH, 0);
      if(rc > 0) {
         printf("send to server success\n");
      }

      printf("Wait for ack from server ...\n");
      memset(buffer,0,BUFFER_LENGTH);
      rc = recv(fd, buffer, BUFFER_LENGTH, 0);
      if(rc > 0) {
         printf("Data from server : %s\n", buffer);
      }

      sleep(3);
   }

   CloseSocket();

   return 0;
}