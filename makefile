CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -pthread
ST_REACTOR_SRC = st_reactor.c
ST_REACTOR_OBJ = $(ST_REACTOR_SRC:.c=.o)
SERVER_SRC = server.c
SERVER_OBJ = $(SERVER_SRC:.c=.o)

all: server

server: $(SERVER_OBJ) $(ST_REACTOR_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -ldl

$(SERVER_OBJ): $(SERVER_SRC)
	$(CC) $(CFLAGS) -c $<

$(ST_REACTOR_OBJ): $(ST_REACTOR_SRC)
	$(CC) $(CFLAGS) -fPIC -c $<

clean:
	rm -f $(SERVER_OBJ) $(ST_REACTOR_OBJ) server

.PHONY: all clean
