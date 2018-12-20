//
//  do_put.c
//  mftp
//
//  Created by mac on 14/12/2018.
//  Copyright © 2018 vladimirburlakov. All rights reserved.
//

#include "do_all.h"
#include "conn.h"
#include "str_func.h"


void
do_put(sockfd fd, char *arg, char *send_buf, char *read_buf) {
    char ip[16];
    int portno;
    ssize_t n;
    char *rbuf;
    int rcode;
    sockfd dcfd; // Data Channel File Descriptor
    
    
    do_pasv(fd, send_buf, read_buf, ip, &portno);
    
    // set binary format for the transfer
    n = send_cmd(fd, "TYPE I", NULL, send_buf);
    if (n < 0)
        exit(1);
    
    rbuf = readl(fd);
    rcode = get_code(rbuf);
    
    if (rcode != 200) {
        handle_error_code_4xx_5xx(rcode, rbuf);
        return;
    }
    
    // DATA CONNECTION !

    dcfd = get_sock(ip, portno);
    
    n = send_cmd(fd, "STOR", arg, send_buf);
    if (n < 0)
        exit(1);
    
    
    rbuf = readl(fd);
    rcode = get_code(rbuf);
    
    // Open local file :
    if (rcode == 150 ) { // OK
        FILE *read_ptr;
        read_ptr = fopen(arg, "rb");
        
        char tmp_buf[SOCK_BUF_SIZE];
        
        int bytes_counter = 0;
        fprintf(stderr, "."); // Just for seeing load process
        ssize_t dn;
        do {
            dn = fread(tmp_buf, sizeof(tmp_buf[0]), SOCK_BUF_SIZE, read_ptr);

            writesock(dcfd, tmp_buf, dn);
            
            bytes_counter += dn;
            if (bytes_counter >= 1048567) {// if bigger that 1Mb 1 * 1024 * 1024
                fprintf(stderr, ".");
                bytes_counter = bytes_counter - 1048567;
            }
            
        } while (dn);
        printf("\n");
        
        
        // close data channel
        close(dcfd);
        
        rbuf = readl(fd);
        
        // TODO here we forgot to check status of the transfer
        printf("%s\n", rbuf);
        
        fclose(read_ptr);
    } else {
        handle_error_code_4xx_5xx(rcode, rbuf);
    }
}
