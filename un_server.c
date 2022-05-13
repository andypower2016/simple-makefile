#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <time.h>

#define SERVER_PATH     "/tmp/server"
#define BUFFER_LENGTH    1024
#define FALSE              0

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

void func1(int fd)
{
   char buffer[BUFFER_LENGTH];
   int rc;
   /* send get sys time */
   memset(buffer, 0, BUFFER_LENGTH);
   strcpy(buffer, "getsystemtime");
   send(fd, buffer, strlen(buffer), 0);
   
   /* recv */
   struct timeval tv;
   tv.tv_sec = 3;
   tv.tv_usec = 0;
   setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*) &tv, sizeof(tv));

   memset(buffer, 0, BUFFER_LENGTH);
   rc = recv(fd, buffer, BUFFER_LENGTH, 0);

   if(rc <= 0) {
      printf("[%s] timeout\n", __FUNCTION__);
   }
   else {
      buffer[rc+1] = '\0';
      printf("[%s] system time = %s\n", __FUNCTION__,buffer);
      
      memset(buffer, 0, BUFFER_LENGTH);
      strcpy(buffer, "ack");
      send(fd, buffer, strlen(buffer), 0);
   }
}


void HandleMessage(int fd, char message[], int len)
{
   char buffer[BUFFER_LENGTH];
   int rc;
   if(strcmp("func1", message) == 0)
   {
      printf("[%s] get message %s\n", __FUNCTION__, message);
      func1(fd);
   }
   else
   {
      printf("[%s] not recognized message\n",__FUNCTION__);
   }
}

void Recieve(int fd, char buffer[]) {

   memset(buffer, 0, BUFFER_LENGTH);
   int rc = recv(fd, buffer, BUFFER_LENGTH, 0);
   if(rc == 0) {
      /* client close connection */
      printf("Client[%d] closed connection\n", fd);
      close(fd);
      FD_CLR(fd, &master); 

   } else if (rc > 0) {

      buffer[rc-1] = '\0';
      printf("Server recv %s from client[%d]\n", buffer, fd);   
      
      HandleMessage(fd, buffer, rc);
      /*memset(buffer,0,BUFFER_LENGTH);
      strcpy(buffer,"server ack");
      send(i, buffer, BUFFER_LENGTH, 0);*/
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
               }
            }  
            else /* handle connected client */
            {
               Recieve(i, buffer);
            }          
         }
      }
   }
   
   CloseServer();

   return 0;
}
