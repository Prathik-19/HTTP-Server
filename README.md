# HTTP Server
## Authors: Shreedhar, Mani, Anish, Prathik

### Overview
This project involves the creation of an HTTP server capable of handling client requests using the HTTP protocol. The server is designed to run indefinitely, maintaining resilience against malformed or malicious client inputs to ensure stability and reliability. It demonstrates the use of client-server modularity and offers a platform for understanding the fundamentals of network programming and system design.

### Features
- **Single-threaded Operation**: Handles client requests one at a time, ensuring straightforward execution flow and easy debugging.
- **Socket Programming**: Utilizes sockets to create, bind, listen, and accept connections on a specified port.
- **HTTP Support**: Processes basic HTTP methods, GET and PUT, to retrieve or update resources on the server.
- **Robust Error Handling**: Designed to manage various client-side errors without crashing, ensuring server stability.

### How It Works
1. **Starting the Server**:
   ```bash
   ./httpserver <port>
   ```
   The server needs a single command-line argument, the port number, to start listening for incoming connections.

2. **Connection Lifecycle**:
   - **Listen**: Initializes a socket to listen for incoming connections.
   - **Accept**: Accepts new client connections and creates a new socket for each connection.
   - **Process**: Reads HTTP requests, processes them according to the HTTP protocol, and sends appropriate responses.
   - **Close**: Closes the connection after responding to the client.

### Technical Setup
- **Implementation Language**: The server is implemented in C, making extensive use of system calls for managing sockets.
- **Error Management**: Implements comprehensive error checking to handle potential runtime failures gracefully.
- **Memory Management**: Ensures efficient memory usage without leaks, maintaining performance integrity.

### Challenges Overcome
- **Concurrency Management**: Handles multiple client requests sequentially without concurrency, simplifying the design and implementation.
- **Security and Stability**: Implements robust error handling to resist potential security threats from malformed requests.

### Future Directions
- **Concurrency Support**: Plans to integrate multi-threading to handle multiple requests simultaneously, improving throughput and efficiency.
- **Extended HTTP Support**: Extend the implementation to support additional HTTP methods and features for a more comprehensive web server experience.

### Usage
- Compile the server using the provided Makefile to ensure all dependencies are correctly configured.
- Run the server on the desired port, monitor its operation, and manage connections using standard HTTP client tools like curl.
