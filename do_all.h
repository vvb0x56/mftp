//
//  do_all.h
//  mftp
//
//  Created by mac on 17/12/2018.
//  Copyright © 2018 vladimirburlakov. All rights reserved.
//

#ifndef do_all_h
#define do_all_h

#include "common.h"

bool do_auth (sockfd fd);

bool do_auth1 (sockfd fd);

void do_dele (sockfd fd, char *arg, char *send_buf, char *read_buf);

void do_rmd (sockfd fd, char *arg, char *send_buf, char *read_buf);

void do_mkd (sockfd fd, char *arg, char *send_buf, char *read_buf);

void do_pwd (sockfd fd, char *send_buf, char *read_buf, char* pwd_buf);

void do_cwd (sockfd fd, char *arg, char *send_buf, char *read_buf);

void do_pasv (sockfd fd, char *send_buf, char *read_buf, char *ip, int *portno);

void do_list (sockfd fd, char *arg, char *send_buf, char *read_buf);

void do_mv (sockfd fd, char *src, char *dst, char *send_buf, char *read_buf);

void do_get (sockfd fd, char *arg, char *send_buf, char *read_buf);

void do_put (sockfd fd, char *arg, char *send_buf, char *read_buf);

#endif /* do_all_h */
