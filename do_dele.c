//
//  do_dele.c
//  mftp
//
//  Created by mac on 17/12/2018.
//  Copyright © 2018 vladimirburlakov. All rights reserved.
//

/*
                         1,3    +---+
                    ----------->| E |
                   |            +---+
                   |
 +---+    cmd    +---+    2      +---+
 | B |---------->| W |---------->| S |
 +---+           +---+           +---+
                   |
                   |     4,5    +---+
                    ----------->| F |
                                +---+
 DELE answer CODEs:
 250
 450, 550
 500, 501, 502, 421, 530

 */

#include "conn.h"
#include "do_all.h"
#include "str_func.h"

void
do_dele(sockfd fd, char *arg, char *send_buf, char *read_buf) {
    
    ssize_t n;
    
    n = send_cmd(fd, "DELE", arg, send_buf);
    
    // if there an error in socket i think we should close application !TODO
    if (n < 0)
        exit(1);
    
    char *rbuf = readl(fd);
    int ret_code = get_code(rbuf);
    
    
    if (is_multi_beg(rbuf))
        handle_ml(fd, rbuf, ret_code);
    
    
    if (ret_code == 250)
        printf("Requested file action okay, completed. File %s deleted.\n", arg);
    else
        handle_error_code_4xx_5xx(ret_code, rbuf);
    
}

