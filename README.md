# c-thread-chat

TCP/IP chat with multi-threads made purely in C. 

## How it works

A host machine needs to compile the `chat_server` and run `chat_server.exe`. After that, the client must set the server’s IP address in `main.c` and compile the `chat_client`. 

Once the server is running, clients can connect as long as the maximum allowed number of clients hasn’t been reached. When a client sends a message to the server, the server broadcasts that message to all currently connected users.

## Project Structure

```
/chat_server
├── src/
│ ├── main.c            # Main server application source code
│ ├── server_net.c      # Network-related functions and logic
│ ├── server_users.c    # User management and handling
│ └── server_utils.c    # Utility functions for the server
├── include/
│ └── server_net.h      # Header file for network functions
│ ├── server_users.h    # Header file for user management
│ └── server_utils.h    # Header file for utility functions
└── Makefile            # Build instructions to compile the project

/chat_client
├── src/
│ ├── main.c            # Main client application source code
│ ├── client_net.c      # Network-related functions and logic
│ └── client_utils.c    # Utility functions for the client
├── include/
│ └── client_net.h      # Header file for network functions
│ └── client_utils.h    # Header file for utility functions
└── Makefile            # Build instructions to compile the project
```

### File and Directory Descriptions

/chat_server
- **main.c**: Entry point of the application. It initializes the server socket using init_server_socket() and begins accepting client connections with accept_client_connections().

- **server_net.c**: Contains core networking functions for starting the server and accepting new client connections. Also includes the broadcast function, client thread handlers, and helper functions used within those threads.

- **server_users.c**: Provides user management functionality, including logic for adding and removing clients, and verifying that usernames are unique.

- **server_utils.c**: Contains utility functions that support the server, such as formatted error messages for Winsock and functions to send connection approval or denial messages to clients.

/chat_client
- **main.c**: Entry point of the application. Initializes the Winsock library, connects to the server, and handles client-side messaging. After connection approval, it sends the client's name, starts a thread to receive messages, and allows the user to send messages via the console until they type "exit" or "disconnect". [!] Important: This is where you should add the server's IP address.

- **client_net.c**: Similar to server_net.c, it handles connecting to the server and includes functions for sending and receiving messages.

- **client_utils.c**: Contains utility functions for the client, including error handling and prompting the user to input a valid name.

## How to Build

Both the server and client directories include a Makefile, which can be compiled using `mingw32-make` (on Windows).

```
To build the server:
cd chat_server
mingw32-make

To build the client:
cd chat_client
mingw32-make

Make sure mingw32-make is installed and available in your system's PATH.

To remove all compiled object files and the executable (clean the build), run:
mingw32-make clean
```
