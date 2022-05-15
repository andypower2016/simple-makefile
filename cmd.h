#ifndef CMD_H
#define CMD_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define BUFFER_LENGTH 4096

typedef struct cmd {

char name[4];
uint32_t type;
uint32_t cmdID;
uint8_t buffer[];
}cmd;

typedef enum cmdType {

	CMD_ID_FUNC1=125,
	CMD_ID_GETSYSTIME,
	CMD_ID_SYSTIME,
	CMD_ID_END,
	CMD_ID_MAX_NUM,
} cmdType;

typedef struct RxBuffer {
   
   uint32_t data;
} RxBuffer;

int SendCommand(int fd, cmdType cmdID, char* data, int datalen)
{
    int sendlen = sizeof(cmd) + datalen;
    cmd *sendCmd = malloc(sendlen);
    uint8_t *sendBuffer = sendCmd->buffer;
    char buffer[BUFFER_LENGTH];
    int rc = 0;
    if(sendCmd)
    {
       sendCmd->cmdID = (uint32_t) cmdID;
       memcpy(sendBuffer, (uint8_t*) data, datalen);
       memcpy(buffer, (char*) sendCmd, sendlen);
       rc = send(fd, buffer, sendlen, 0);
       if(rc > 0) {
         printf("[%s] sendCmd=%d\n",__FUNCTION__,cmdID);
       }
       free(sendCmd);
    }
    return rc;
}

#endif
