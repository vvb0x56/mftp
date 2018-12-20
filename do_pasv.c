//
//  do_pasv.c
//  mftp
//
//  Created by mac on 14/12/2018.
//  Copyright © 2018 vladimirburlakov. All rights reserved.
//

#include "do_all.h"
#include "conn.h"
#include "str_func.h"

void
do_pasv(sockfd fd, char *send_buf, char *read_buf, char *ip, int *portno){
    //memset(send_buf, 0, 1);
    ssize_t n;
    char *rbuf;
    int rcode;
    bool is_ml;
    
    n = send_cmd(fd, "PASV", NULL, send_buf);
    if (n < 0)
        exit(1);
    
    rbuf = readl(fd);
    rcode = get_code(rbuf);
    
    is_ml = is_multi_beg(rbuf);
    
    //Before other operations, we should handle success answer, cause futher read buf maybe changed f.e if multiline answer
    if (rcode == 227) {
        // 227 Entering Passive Mode (10,199,30,10,218,123). `x * 256 + y`
        char *ipport_tok;
        char *ip_tok;
        int x, y;
        
        ipport_tok = strtok(rbuf, "()");
        ipport_tok = strtok(NULL, "()");
        //printf("%s\n", ipport_tok);
        
        //char ip[16]; clean ip buf
        ip[0] = '\0';
        ip_tok = strtok(ipport_tok, ",");
        strcat(ip, ip_tok);
        for (int i = 0; i < 3; i++) {
            strcat(ip, ".");
            ip_tok = strtok(NULL, ",");
            strcat(ip, ip_tok);
        }
        
        x = atoi(strtok(NULL, ","));
        y = atoi(strtok(NULL, ","));
        
        *portno = x * 256 + y;
        //printf("port num: %d\n", portno);
        
        
        if (!is_ml)
            return;
    }
    
    // MULTILINE !!!!
    // im not sure what we should expect from multiline, so now i just print it
    if (is_ml)
        handle_ml(fd, rbuf, rcode);

    
    /////////////
    
    if (rcode != 227)
        handle_error_code_4xx_5xx(rcode, rbuf);
}

