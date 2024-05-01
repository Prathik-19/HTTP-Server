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

#define REQEX  "^([a-zA-Z]{1,8}) /([a-zA-Z0-9.-]{1,63}) (HTTP/[0-9]\\.[0-9])\r\n"
#define HEADEX "([a-zA-Z0-9.-]{1,128}): ([ -~]{1,128})\r\n"

// get()
int get(Request *req) {
    // Check for bad request conditions
    if (req->cont_len != -1 || 0 < req->bytes_remaining) {
        dprintf(req->infile_d,
            "HTTP/1.1 400 Bad Request\r\nContent-Length: %d\r\n\r\nBad Request\n", 12);
        return (EXIT_FAILURE);
    }

    int file_d;
    // Check if the requested path is a directory
    if ((file_d = open(req->path, O_RDONLY | O_DIRECTORY)) != -1) {
        dprintf(
            req->infile_d, "HTTP/1.1 403 Forbidden\r\nContent-Length: %d\r\n\r\nForbidden\n", 10);
        return (EXIT_FAILURE);
    }

    // Open the file for reading
    if ((file_d = open(req->path, O_RDONLY)) == -1) {
        // File not found
        if (ENOENT == errno) {
            dprintf(req->infile_d,
                "HTTP/1.1 404 Not Found\r\nContent-Length: %d\r\n\r\nNot Found\n", 10);
            return (EXIT_FAILURE);
        }
        // Permission denied
        if (EACCES == errno) {
            dprintf(req->infile_d,
                "HTTP/1.1 403 Forbidden\r\nContent-Length: %d\r\n\r\nForbidden\n", 10);
            return (EXIT_FAILURE);
        } else {
            dprintf(req->infile_d,
                "HTTP/1.1 500 Internal Server Error\r\nContent-Length: %d\r\n\r\nInternal Server "
                "Error\n",
                21);
            return (EXIT_FAILURE);
        }
    }

    struct stat file_stat;
    fstat(file_d, &file_stat);

    dprintf(req->infile_d, "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\n\r\n", file_stat.st_size);

    // Pass file content to the client
    int written_bytes = pass_n_bytes(file_d, req->infile_d, file_stat.st_size);
    if (-1 == written_bytes) {
        // Error occurred while sending file content
        dprintf(req->infile_d,
            "HTTP/1.1 500 Internal Server Error\r\nContent-Length: %d\r\n\r\nInternal Server "
            "Error\n",
            21);
        return (EXIT_FAILURE);
    }
    close(file_d);
    return (EXIT_SUCCESS);
}

// put()
int put(Request *req) {
    // Check if content length is valid
    if (req->cont_len == -1) {
        // Send Bad Request response if content length is invalid
        dprintf(req->infile_d,
            "HTTP/1.1 400 Bad Request\r\nContent-Length: %d\r\n\r\nBad Request\n", 12);
        return EXIT_FAILURE;
    }

    int file_d;
    int status;

    // Attempt to open file for writing exclusively
    if (-1 == (file_d = open(req->path, O_WRONLY | O_CREAT | O_EXCL, 0666))) {
        // Handle file already exists case
        if (EEXIST == errno) {
            // Attempt to open file for truncation
            if (-1 != (file_d = open(req->path, O_WRONLY | O_TRUNC, 0666))) {
                status = 200; // Set status to 200 OK
            } else {
                // Send 403 error response if unable to open file for truncation
                dprintf(req->infile_d,
                    "HTTP/1.1 403 Forbidden\r\nContent-Length: %d\r\n\r\nForbidden\n", 10);
                return EXIT_FAILURE;
            }
        } else if (EACCES == errno) {
            // Send 403  response error if access is denied
            dprintf(req->infile_d,
                "HTTP/1.1 403 Forbidden\r\nContent-Length: %d\r\n\r\nForbidden\n", 10);
            return EXIT_FAILURE;
        } else {
            // Send 500 Error response for other errors
            dprintf(req->infile_d,
                "HTTP/1.1 500 Internal Server Error\r\nContent-Length: %d\r\n\r\nInternal Server "
                "Error\n",
                21);
            return EXIT_FAILURE;
        }
    } else {
        // Set status to 201 Created if file is created successfully
        status = (errno == EEXIST) ? 200 : 201;
    }

    char *message = (status == 201) ? "Created\n" : "OK\n";

    dprintf(req->infile_d, "HTTP/1.1 %d %s\r\nContent-Length: %zu\r\n\r\n%s", status,
        (status == 201) ? "Created" : "OK", strlen(message), message);

    // Write message content to file if status indicates success
    if (status == 200) {
        int written_bytes = write_n_bytes(file_d, req->message, req->cont_len);
        if (written_bytes == -1) {
            dprintf(req->infile_d,
                "HTTP/1.1 500 Internal Server Error\r\nContent-Length: %d\r\n\r\nInternal Server "
                "Error\n",
                21);
            close(file_d);
            return EXIT_FAILURE;
        }
    } else if (status == 201) {
        int written_bytes = write_n_bytes(file_d, req->message, req->cont_len);
        if (written_bytes == -1) {
            dprintf(req->infile_d,
                "HTTP/1.1 500 Internal Server Error\r\nContent-Length: %d\r\n\r\nInternal Server "
                "Error\n",
                21);
            close(file_d);
            return EXIT_FAILURE;
        }
    }

    close(file_d);
    return EXIT_SUCCESS;
}
