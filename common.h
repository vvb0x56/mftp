//
//  common.h
//  mftp
//
//  Created by mac on 14/12/2018.
//  Copyright © 2018 vladimirburlakov. All rights reserved.
//

#ifndef common_h
#define common_h

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include <limits.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <limits.h>

typedef int bool;
#define true 1
#define false 0

typedef int sockfd;

char *
get_pass(char *pass_buf, int buf_size);

char *
read_line(sockfd fd);

typedef struct {
    struct hashnode** ht;
    char *inputbuf;
    char *sendbuf;
    char *readbuf;
    char *pwdbuf;
    char *tmpbuf;
    
} resources;

#endif /* common_h */
