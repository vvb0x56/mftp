//
//  conn.h
//  mftp
//
//  Created by mac on 10/12/2018.
//  Copyright © 2018 vladimirburlakov. All rights reserved.
//

#ifndef conn_h
#define conn_h

#include "common.h"

extern const int BUF_SIZE;
extern const int SOCK_BUF_SIZE;
extern const int SOCK_BUF_SIZE;

int
get_sock(const char *hostname, int portno);

ssize_t
mread(int fd, char *buf, int bs);

ssize_t
mwrite(int fd, char *buf, ssize_t bs);

ssize_t readln(sockfd fd, char *buf);


ssize_t
readsock(int fd, char *buf, int bs);

ssize_t
writesock(int fd, char *buf, ssize_t bs);

char *
assemble_line(char *dst, char *src, ssize_t n);

ssize_t
increase_strbuf_size(char **str, ssize_t size);

ssize_t send_cmd(sockfd fd, char *cmd, char *arg, char *buf);

char * addc(char* dst, char c);


char readc(sockfd fd);

char *readl(sockfd fd);

void handle_ml(sockfd fd, char *s, int code);

#endif /* conn_h */
