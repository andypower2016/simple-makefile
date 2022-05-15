#ifndef CMD_H
#define CMD_H

#include <stdint.h>

typedef struct cmd {

char name[4];
uint32_t type;
uint32_t cmdID;

}cmd;


#endif
