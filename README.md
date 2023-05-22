# Reactor Pattern Server

This is a simple implementation of a server application using the Reactor pattern. The Reactor pattern allows handling multiple concurrent client connections using a single thread.


## Overview

The code consists of several components and functions:
- `Client` struct: Represents a client connected to the server.
- `Reactor` struct: Represents the reactor, which manages the clients and handles events.
- `createReactor`: Creates and initializes a new reactor.
- `handle_new_connection`: Handles a new client connection.
- `handle_client_disconnection`: Handles a client disconnection.
- `handle_client_message`: Handles a message received from a client.
- `startReactor`: Starts the reactor event loop.
- `addFd`: Adds a new file descriptor and its associated handler to the reactor.
- `reactorThread` and `startReactorThread`: Functions to run the reactor event loop in a separate thread.
- `main`: The main function sets up the server socket, creates the reactor, adds file descriptors and their handlers, and starts the reactor in a separate thread.


## Prerequisites

To compile and run the code, you need a C compiler and the necessary development libraries for socket programming.


## Getting Started

1. Clone the repository or download the source code files.
2. make all
3. ./react_server
