//
//  do_auth.c
//  mftp
//
//  Created by mac on 14/12/2018.
//  Copyright © 2018 vladimirburlakov. All rights reserved.
//

#include "do_all.h"
#include "conn.h"
#include "str_func.h"

extern const int CMD_SIZE;
extern const int MAX_PASSW_LEN;

extern resources res;

bool do_auth(fd) {
    
    ssize_t n;
    bool is_logged = false;
    
    char *rbuf;
    char *tbuf = res.tmpbuf;
    
    int rcode;
    //char *tbuf = res.tmpbuf;
    

    
    while (!is_logged) {
        rbuf = readl(fd);
        printf("%s\n", rbuf);
        
        rcode = get_code(rbuf);
        
        switch (rcode) {
            case 500:
            case 530:
                // err
                fprintf(stderr, "Login incorrect. Try again.\n");
            case 220:
                // send USER user\r\n information to the server
                do {
                    printf("User: ");
                    fgets(tbuf, SOCK_BUF_SIZE, stdin);
                } while (strlen(tbuf) == 1);    // at least on char should be except '\n'
                    
                clean_r_n(tbuf);
                
                n = send_cmd(fd, "USER", tbuf, res.sendbuf);
                if ( n < 0 )
                    exit(1);
                break;
            case 331:
                // send PASS password to the server
                do {
                    printf("Password: ");
                
                    if (!get_pass(tbuf, MAX_PASSW_LEN)) {
                        printf("Cannot hide password input, be warn!\n");
                        fgets(tbuf, 1024, stdin);
                    }
                } while (strlen(tbuf) == 1); // password should not be empty, we check if there only '\n' char
                
                
                clean_r_n(tbuf);
                
                n = send_cmd(fd, "PASS", tbuf, res.sendbuf);
                if ( n < 0 )
                    exit(1);
                
                //attempts++;
                break;
            case 230:
                fprintf(stderr, "Got an access to the server\n");
                is_logged = true;
                continue;// OK we logged in
            default:
                fprintf(stderr, "!!! Unknown answer from the server: %s\n", rbuf);
                break;
        }
    }
    
    return is_logged;
}

bool
do_auth1(sockfd fd) {
    ssize_t n = 0;
    char *rbuf = (char *)malloc(BUF_SIZE);
    char *wbuf = (char *)malloc(BUF_SIZE);
    char *tbuf = (char *)malloc(BUF_SIZE); // temporary buffer
    
    if (!rbuf || !wbuf || !tbuf) {
        perror("Error to allocate mem in do_auth func.\n");
        exit(1);
    }
    
    char *line = NULL;
    char cmd[CMD_SIZE + 1];
    // int attempts = 0;
    ssize_t tbuf_size = BUF_SIZE;
    bool islogged = false;
    
    
    memset(rbuf, 0, BUF_SIZE);
    
    
    
    // while (attempts != MAX_AUTH_ATTEMPTS) {
    //       && !islogged) {
    while (!islogged) {
        // read from server
        // here we are trying to get whole line
        do {
            n = readsock(fd, rbuf, BUF_SIZE - 1);
            if ((tbuf_size - strlen(tbuf) + 1) < n) {
                tbuf_size = increase_strbuf_size(&tbuf, tbuf_size);
            }
            line = assemble_line(tbuf, rbuf, n);
            rbuf[n] = '\0';
        } while (!line); // Trying to get full line with \n from server
        
        printf("%s\n", line);
        // consider what we should do depends on server request
        int code = get_code(line);
        switch (code) {
            case 500:
            case 530:
                // err
                fprintf(stderr, "Login incorrect. Try again.\n");
            case 220:
                // send USER user\r\n information to the server
                strcpy(cmd, "USER");
                printf("User: ");
                fgets(wbuf, 1024, stdin);
                break;
            case 331:
                // send PASS password to the server
                strcpy(cmd, "PASS");
                printf("Password: ");
                
                if (!get_pass(wbuf, MAX_PASSW_LEN)) {
                    printf("Cannot hide password input, be warn!\n");
                    fgets(wbuf, 1024, stdin);
                }
                
                //attempts++;
                break;
            case 230:
                fprintf(stderr, "Got an access to the server\n");
                islogged = true;
                continue;// OK we logged in
                default:
                fprintf(stderr, "!!! Unknown answer from the server: %s", line);
                break;
        }
        
        // send to the server if still not logged, added that to free(line) if logged.
        if (!islogged) {
            cmdcat(cmd, wbuf);
            writesock(fd, wbuf, strlen(wbuf));
        }
        if (line)   free(line);
        line = NULL;
    }
    free(tbuf);
    free(rbuf);
    free(wbuf);
    return islogged;
}
