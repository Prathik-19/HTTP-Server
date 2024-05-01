#pragma once

#include "asgn2_helper_funcs.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <regex.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

// Request Struct
typedef struct Request {
    char *comm;
    char *path;
    char *vers;
    char *message;
    int infile_d;
    int cont_len;
    int bytes_remaining;
} Request;

// Request Handling
int req_handler(Request *req);

// Request Parsing
int req_parser(Request *req, char *buff, ssize_t completed_read);
