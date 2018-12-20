//
//  common.c
//  mftp
//
//  Created by mac on 14/12/2018.
//  Copyright © 2018 vladimirburlakov. All rights reserved.
//

#include "common.h"
#include "conn.h"

//const int BUF_SIZE_TMP = 8;
extern const int BUF_SIZE;
extern const int SOCK_BUF_SIZE;
extern const int MAX_PASSW_LEN;
//const int MAX_AUTH_ATTEMPTS = 3;
extern const int CMD_SIZE;;
extern const int HASH_TABLE_SIZE;;
extern const int MAX_USER_INPUT_ELEM_NUM;

char *
get_pass(char *pass_buf, int buf_size) {
    
    struct termios old_term, new_term;
    
    if (tcgetattr(STDIN_FILENO, &old_term) != 0)
        return NULL;
    new_term = old_term;
    new_term.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_term) != 0)
        return NULL;
    
    char *rv;
    rv = fgets(pass_buf, buf_size, stdin);
    
    (void) tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_term);
    
    return rv;
}

char *
read_line(sockfd fd) {
    ssize_t n = 0;
    char *rbuf = (char *)malloc(BUF_SIZE);
    char *wbuf = (char *)malloc(BUF_SIZE);
    char *tbuf = (char *)malloc(BUF_SIZE); // temporary buffer
    
    if (!rbuf || !wbuf || !tbuf) {  //  Check if nill
        perror("Error to allocate mem in do_auth func.\n");
        exit(1);
    }
    
    ssize_t tbuf_size = BUF_SIZE;
    
    
    char *line = NULL;
    
    do {
        n = readsock(fd, rbuf, BUF_SIZE - 1);
        if ((tbuf_size - strlen(tbuf) + 1) < n) {
            tbuf_size = increase_strbuf_size(&tbuf, tbuf_size);
        }
        line = assemble_line(tbuf, rbuf, n);
        //       rbuf[n] = '\0';
        //       printf("rbuf: %s \\n \n", rbuf);
    } while (!line);
    
    free(rbuf);
    free(wbuf);
    free(tbuf);
    
    return line;
}

