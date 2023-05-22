CC = gcc
CFLAGS = -Wall -Wextra -g -fPIC
LDFLAGS = -pthread
ST_REACTOR_SRC = st_reactor.c
ST_REACTOR_OBJ = $(ST_REACTOR_SRC:.c=.o)
SERVER_SRC = server.c
SERVER_OBJ = $(SERVER_SRC:.c=.o)

all: react_server st_reactor.so

react_server: $(SERVER_OBJ) $(ST_REACTOR_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -ldl

st_reactor.so: $(ST_REACTOR_OBJ)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS)

$(SERVER_OBJ): $(SERVER_SRC)
	$(CC) $(CFLAGS) -c $<

$(ST_REACTOR_OBJ): $(ST_REACTOR_SRC)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(SERVER_OBJ) $(ST_REACTOR_OBJ) react_server st_reactor.so

.PHONY: all clean
