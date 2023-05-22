# Makefile

CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS =

# Source files
SERVER_SRC = st_reactor.c

# Object files
SERVER_OBJ = $(SERVER_SRC:.c=.o)

# Executables
SERVER_EXEC = react_server

all: $(SERVER_EXEC)

$(SERVER_EXEC): $(SERVER_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f  $(SERVER_EXEC)  $(SERVER_OBJ)
