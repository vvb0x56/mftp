
//
//  do_cwd.c
//  mftp
//
//  Created by mac on 14/12/2018.
//  Copyright © 2018 vladimirburlakov. All rights reserved.
//

#include "do_all.h"
#include "conn.h"
#include "str_func.h"

void
do_cwd(sockfd fd, char *arg, char *send_buf, char *read_buf) {
    ssize_t n;
    char *rbuf;
    int rcode;
    //memset(send_buf, 0, 1); // clean send_buf
    
    n = send_cmd(fd, "CWD", arg, send_buf);
    if (n < 0)
        exit(1); //it's seems there we should close the connection and exit from program
    
    rbuf = readl(fd); // TODO handle socker read error from the function
    rcode = get_code(rbuf);
    
    
    
    // MULTILINE !!!!
    // im not sure what we should expect from multiline, so now i just print it
    if (is_multi_beg(rbuf)) {
        handle_ml(fd, rbuf, rcode);
    }
    /////////////
    
    if (rcode != 250)
        handle_error_code_4xx_5xx(rcode, rbuf);
}
