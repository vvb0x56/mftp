//
//  do_pwd.c
//  mftp
//
//  Created by mac on 14/12/2018.
//  Copyright © 2018 vladimirburlakov. All rights reserved.
//

#include "do_all.h"
#include "conn.h"
#include "str_func.h"

void
do_pwd(sockfd fd, char *send_buf, char *read_buf, char* pwd_buf) {
    memset(pwd_buf, 0, 1);
    
    ssize_t n;
    char *rbuf;
    int rcode;
    //memset(send_buf, 0, 1); // clean send_buf
    n = send_cmd(fd, "PWD", NULL, send_buf);
    if (n < 0)
        exit(1);
    
    rbuf = readl(fd);
    rcode = get_code(rbuf);
    
    // MULTILINE !!!!
    // im not sure what we should expect from multiline, so now i just print i
    if (is_multi_beg(rbuf))
        handle_ml(fd, rbuf, rcode);
    
    /////////////
    
    if (rcode == 257)
        pwd_parse(rbuf, pwd_buf);
    else
        handle_error_code_4xx_5xx(rcode, rbuf);
}
