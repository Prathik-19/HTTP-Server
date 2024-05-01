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
#include "asgn2_helper_funcs.h"

#define REQEX  "^([a-zA-Z]{1,8}) /([a-zA-Z0-9.-]{1,63}) (HTTP/[0-9]\\.[0-9])\r\n"
#define HEADEX "([a-zA-Z0-9.-]{1,128}): ([ -~]{1,128})\r\n"

#ifndef HTTPCOMMANDS_H
#define HTTPCOMMANDS_H

#include "httprequest.h"

// Function declarations
int get(Request *req);
int put(Request *req);

#endif /* HTTPCOMMANDS_H */
