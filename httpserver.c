#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <regex.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#include "httprequest.h"
#include "httpcommands.h"
#include "asgn2_helper_funcs.h"

#define Max_Buf 4096

int main(int argc, char *argv[]) {
    // Check if the correct number of arguments is provided
    if (2 != argc) {
        return (EXIT_FAILURE);
    }

    // Initialize a buffer for reading data
    char buff[1 + Max_Buf] = { '\0' };

    Listener_Socket sock;

    // Extract port number from command line argument
    int port = strtol(argv[1], NULL, 10);
    if (errno == EINVAL) {
        fprintf(stderr, "Invalid Port\n");
        return (EXIT_FAILURE);
    }

    // Initialize the listener socket
    int socket_status = listener_init(&sock, port);
    if (-1 == socket_status) {
        fprintf(stderr, "Invalid Port\n");
        return (EXIT_FAILURE);
    }

    // Continuously accept connections and process requests
    while (true) {
        // Accept incoming connection and get the file descriptor
        int file_d = listener_accept(&sock);
        if (-1 == file_d) {
            fprintf(stderr, "Unable to Connect\n");
            return (EXIT_FAILURE);
        }

        // Initialize a Request structure for handling the request
        Request req;
        req.cont_len = -1;
        req.infile_d = file_d;

        // Read data from the client
        ssize_t bytes_read = read_n_bytes(file_d, buff, Max_Buf);

        // Parse the HTTP request
        if (req_parser(&req, buff, bytes_read) != EXIT_FAILURE) {
            req_handler(&req);
        }
        close(file_d);
        memset(buff, '\0', sizeof(buff));
    }
    return (EXIT_SUCCESS);
}
