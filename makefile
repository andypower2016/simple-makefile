CC      := gcc
INC     := -I"./include"
CFLAGS  := -O -Wall -pthread $(INC)

SRC 	:= ./src
SRCS    := $(shell find $(SRC) -name '*.c')

OBJ 	:= ./obj
OBJS    := $(SRCS:%.c=%.o)

BIN 	:= main
LDLIBS	:= -lm
MKDIR   := mkdir
RMDIR   := rmdir

all:$(OBJS) 
	$(CC) -o $(BIN) $^ $(CFLAGS) $(LDLIBS)
	@echo make target [$@] is complete

%o: %c
	$(CC) -c $< -o $@ $(CFLAGS) 
	@echo make target [$@] is complete

clean:
	-rm -f $(BIN) $(OBJS)



