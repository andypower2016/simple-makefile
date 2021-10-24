CC      := gcc
INC     := -I"./include"
CFLAGS  := -O -Wall -pthread $(INC)
SRC 	:= ./src
SRCS 	:= $(wildcard *.c $(SRC)/*.c)
OBJ 	:= ./obj
OBJS 	:= $(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(SRCS))
BIN 	:= main
LDLIBS	:= -lm

MKDIR   := mkdir
RMDIR   := rmdir

all:$(OBJS)
	$(CC) -o $(BIN) $^ $(CFLAGS) $(LDLIBS)
	@echo make target [$@] is complete

$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) -o $@ -c $< $(CFLAGS)
	@echo make target [$@] is complete

$(OBJ):
	$(MKDIR) $(OBJ)

clean:
	-rm -f $(BIN)
	-rm -r $(OBJ)

