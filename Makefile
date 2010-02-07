BIN	= r3pong
SRC	= main.c
CFLAGS	= -std=c99 -Wall -O0
LDFLAGS	= -lglut
RM	= rm -f
CC	= gcc

OBJS	= $(SRC:%.c=%.o)

all:	$(BIN)

$(BIN):	$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

$.o:	%.c
	$(CC) $(CFLAGS) -c $<

run:	$(BIN)
	./$(BIN)

clean:
	$(RM) $(BIN) $(OBJS)
