//
//  do_list.c
//  mftp
//
//  Created by mac on 14/12/2018.
//  Copyright © 2018 vladimirburlakov. All rights reserved.
//

#include "do_all.h"
#include "str_func.h"
#include "conn.h"

void
do_list(sockfd fd, char *arg, char *send_buf, char *read_buf){
    
    char ip[16]; // 16 - max char in possible ip address in format xxx.xxx.xxx.xxx\0
    char *rbuf;
    int portno;
    int rcode;
    ssize_t n;
    
    do_pasv(fd, send_buf, read_buf, ip, &portno);
    
    sockfd dcfd;
    
    dcfd = get_sock(ip, portno);  // DATA CHANNEL FD
    
    n = send_cmd(fd, "LIST", arg, send_buf);
    if (n < 0)
        exit(1); //it's seems there we should close the connection and exit from program
    
    
    rbuf = readl(fd);
    rcode = get_code(rbuf);
    
    
    if (rcode == 125 || rcode == 150)
        printf("Opening ASCII mode data connection for file list\n");
    else {
        handle_error_code_4xx_5xx(rcode, rbuf);
        return;
    }
    
    char tmp_buf[SOCK_BUF_SIZE];
    
    ssize_t dn = mread(dcfd, tmp_buf, SOCK_BUF_SIZE - 1); // for printf we should use ending \0..
    while (dn > 0) {
        tmp_buf[dn] = '\0';
        printf("%s", tmp_buf);
        dn = mread(dcfd, tmp_buf, SOCK_BUF_SIZE - 1);
    }
    
    if (dn < 0) {
        perror("error: ");
        exit(1);
    }
    
    rbuf = readl(fd);
    rcode = get_code(rbuf);
    
    
    if (rcode == 226 || rcode == 250) {
        printf("Closing data connection.\n");
    } else
        handle_error_code_4xx_5xx(rcode, rbuf);
    
    //  !!!!!I believe we didn't expect multiline while working with data connection ?!//
    
}
