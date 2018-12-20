
//
//  do_mv.c
//  mftp
//
//  Created by mac on 14/12/2018.
//  Copyright © 2018 vladimirburlakov. All rights reserved.
//

#include "do_all.h"
#include "conn.h"
#include "str_func.h"

void
do_mv(sockfd fd, char *src, char *dst, char *send_buf, char *read_buf) {
    ssize_t n;
    char *rbuf;
    int rcode;
    
    ////////////////// RNFR ////////////////////////
    n = send_cmd(fd, "RNFR", src, send_buf);
    if (n < 0)
        exit(1);
    
    
    rbuf = readl(fd);
    rcode = get_code(rbuf);
    
    if (rcode != 350){
        handle_error_code_4xx_5xx(rcode, rbuf);
        return;
    }
    
    ////////////////////////////////?RNTO ///////////////////////
    
    n = send_cmd(fd, "RNTO", dst, send_buf);
    if (n < 0)
        exit(1);
    
    rbuf = readl(fd);
    rcode = get_code(rbuf);
    
    
    if (rcode != 250) {
        handle_error_code_4xx_5xx(rcode, rbuf);
    }
    
}
