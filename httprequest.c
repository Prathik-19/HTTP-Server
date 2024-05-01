#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <regex.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#include "httpcommands.h"
#include "httprequest.h"
#include "asgn2_helper_funcs.h"

#define REQEX  "^([a-zA-Z]{1,8}) /([a-zA-Z0-9.-]{1,63}) (HTTP/[0-9]\\.[0-9])\r\n"
#define HEADEX "([a-zA-Z0-9.-]{1,128}): ([ -~]{1,128})\r\n"

// Function to handle the HTTP request
int req_handler(Request *req) {
    // Check if the HTTP version is supported
    if (strncmp(req->vers, "HTTP/1.1", 8) != 0) {
        // Respond with a 505 error if the version is not supported
        dprintf(req->infile_d,
            "HTTP/1.1 505 Version Not Supported\r\nContent-Length: %d\r\n\r\nVersion Not "
            "Supported\n",
            22);
        return (EXIT_FAILURE);
    }
    // Check if the command is GET
    else if (strncmp(req->comm, "GET", 3) == 0) {
        return (get(req));
    }
    // Check if the command is PUT
    else if (strncmp(req->comm, "PUT", 3) == 0) {
        return (put(req));
    }
    // If the command is not GET or PUT, respond with a 501 error
    else {
        dprintf(req->infile_d,
            "HTTP/1.1 501 Not Implemented\r\nContent-Length: %d\r\n\r\nNot Implemented\n", 16);
    }
    return (EXIT_FAILURE);
}

// Function to parse the HTTP request
int req_parser(Request *req, char *buff, ssize_t completed_read) {
    regex_t regex_compiler;
    regmatch_t number_of_matches[4];
    int regex_result, offset = 0;

    // Compile and execute the regular expression for parsing the request line
    regex_result = regcomp(&regex_compiler, REQEX, REG_EXTENDED);
    regex_result = regexec(&regex_compiler, buff, 4, number_of_matches, 0);

    // If the regular expression matches
    if (regex_result == 0) {
        // Set command, path, and version based on matches
        req->comm = buff;
        req->path = buff + number_of_matches[2].rm_so;
        req->vers = buff + number_of_matches[3].rm_so;

        // Null-terminate the strings
        buff[number_of_matches[1].rm_eo] = '\0';
        req->path[number_of_matches[2].rm_eo - number_of_matches[2].rm_so] = '\0';
        req->vers[number_of_matches[3].rm_eo - number_of_matches[3].rm_so] = '\0';

        // Update buffer and offset
        buff += number_of_matches[3].rm_eo + 2;
        offset += number_of_matches[3].rm_eo + 2;

        // Respond with a 400 error if the request line is invalid
    } else {
        dprintf(req->infile_d,
            "HTTP/1.1 400 Bad Request\r\nContent-Length: %d\r\n\r\nBad Request\n", 12);
        regfree(&regex_compiler);
        return (EXIT_FAILURE);
    }

    // Initialize cont_len and compile and execute the header regular expression
    req->cont_len = -1;
    regex_result = regcomp(&regex_compiler, HEADEX, REG_EXTENDED);
    regex_result = regexec(&regex_compiler, buff, 3, number_of_matches, 0);

    // Process each header
    while (regex_result == 0) {
        buff[number_of_matches[1].rm_eo] = '\0';
        buff[number_of_matches[2].rm_eo] = '\0';

        // Check if the header is the same as the Content-Length
        if (strncmp(buff, "Content-Length", 14) == 0) {
            int value = strtol(buff + number_of_matches[2].rm_so, NULL, 10);
            if (errno == EINVAL) {
                dprintf(req->infile_d,
                    "HTTP/1.1 400 Bad Request\r\nContent-Length: %d\r\n\r\nBad Request\n", 12);
            }
            req->cont_len = value;
        }

        // Update buffer and offset
        buff += number_of_matches[2].rm_eo + 2;
        offset += number_of_matches[2].rm_eo + 2;

        regex_result = regexec(&regex_compiler, buff, 3, number_of_matches, 0);
    }
    // Check if there is a message body
    if ((regex_result != 0) && (buff[0] == '\r' && buff[1] == '\n')) {
        req->message = buff + 2;
        offset += 2;
        req->bytes_remaining = completed_read - offset;

        // If there is no message body, respond with a 400 error
    } else if (regex_result != 0) {
        dprintf(req->infile_d,
            "HTTP/1.1 400 Bad Request\r\nContent-Length: %d\r\n\r\nBad Request\n", 12);
        regfree(&regex_compiler);
        return (EXIT_FAILURE);
    }
    regfree(&regex_compiler);
    return (EXIT_SUCCESS);
}
