//
//  do_get.c
//  mftp
//
//  Created by mac on 14/12/2018.
//  Copyright © 2018 vladimirburlakov. All rights reserved.
//

#include "do_all.h"
#include "conn.h"
#include "str_func.h"

void
do_get(sockfd fd, char *arg, char *send_buf, char *read_buf) {
    char ip[16];
    char *rbuf;
    int portno;
    int rcode;
    ssize_t n;
    
    do_pasv(fd, send_buf, read_buf, ip, &portno);
    
    n = send_cmd(fd, "TYPE I", NULL, send_buf);
    if (n < 0)
        exit(1);
    
    rbuf = readl(fd); // TODO handle socker read error from the function
    rcode = get_code(rbuf);
    
    if (rcode != 200) {
        handle_error_code_4xx_5xx(rcode, rbuf);
        return;
    }
    
    sockfd dcfd;
    
    dcfd = get_sock(ip, portno);
    
    n = send_cmd(fd, "RETR", arg, send_buf);
    if (n < 0)
        exit(1);
    
    rbuf = readl(fd);
    rcode = get_code(rbuf);
    
    
    // Write to local file
    if (rcode == 150) {
        FILE *write_ptr;
        write_ptr = fopen(arg, "wb");
        
        char tmp_buf[SOCK_BUF_SIZE];
        int bytes_counter = 0;
        ssize_t dn = readsock(dcfd, tmp_buf, SOCK_BUF_SIZE);
        fprintf(stderr, ".");
        while (dn) {
            fwrite(tmp_buf, sizeof(tmp_buf[0]), dn, write_ptr);
            
            // will show '.' after 1M was loaded
            bytes_counter += dn;
            if (bytes_counter >= 1048567) {// bigger that 1Mb 1 * 1024 * 1024
                fprintf(stderr, ".");
                bytes_counter = bytes_counter - 1048567;
            }
            
            dn = readsock(dcfd, tmp_buf, SOCK_BUF_SIZE);
        }
        printf("\n");
        
        rbuf = readl(fd);
        // TODO here we forgot to check status of the transfer
        printf("%s\n", rbuf);
        
        //close data socket
        close(dcfd);
        //flush and close file
        fflush(write_ptr);
        fclose(write_ptr);
        //close(nfd);
    } else  {
        handle_error_code_4xx_5xx(rcode, rbuf);
    }
}

