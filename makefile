# Makefile

CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS =

# Source files
CLIENT_SRC = client.c
SERVER_SRC = st_reactor.c

# Object files
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)
SERVER_OBJ = $(SERVER_SRC:.c=.o)

# Executables
CLIENT_EXEC = client
SERVER_EXEC = server

all: $(CLIENT_EXEC) $(SERVER_EXEC)

$(CLIENT_EXEC): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(SERVER_EXEC): $(SERVER_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(CLIENT_EXEC) $(SERVER_EXEC) $(CLIENT_OBJ) $(SERVER_OBJ)
