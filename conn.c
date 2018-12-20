//
//  conn.c
//  mftp
//
//  Created by mac on 10/12/2018.
//  Copyright © 2018 vladimirburlakov. All rights reserved.
//

#include "conn.h"
#include "str_func.h"

extern resources res;


int get_sock(const char *hostname, int portno) {
    int fd;
    // try to  create a socket
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("Error while open socket.");
        exit(1);
    }
    
    // FILL THE HOSTENT STRUCTURE
    /*
     struct  hostent {
     char    *h_name;        official name of host
     char    **h_aliases;    alias list
     int     h_addrtype;     host address type
     int     h_length;       length of address
     char    **h_addr_list;  list of addresses from name server
     #define h_addr  h_addr_list[0]  address, for backward compatiblity
     };
     */
    struct hostent *server_p;
    struct sockaddr_in server_addr;
    
    server_p = gethostbyname(hostname);
    if (server_p == NULL) {
        perror("Error wrong host name.");
        exit(1);
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portno);
    memcpy(&server_addr.sin_addr.s_addr, server_p->h_addr, sizeof(server_p->h_length));
    
    if (connect(fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <  0) {
        perror("Error: ");
        exit(1);
    }
    return fd;
}

ssize_t mread(int fd, char *buf, int bs) {
    ssize_t n;
    do {
        n =  read(fd, (void *)buf, bs);
        
    } while ( n < 0 && (errno == EINTR || errno == EAGAIN));
    
    if (n < 0) {
        perror("Error while read from socket.");
    }
    return n;
}

ssize_t mwrite(int fd, char *buf, ssize_t bs) {
    ssize_t n;
    do {
        n = write(fd, buf, bs);
    } while (n < 0 && (errno == EINTR || errno ==  EAGAIN));
    
    if (n < 0) {
        perror("Write function returned negative value. Shoul we continue ?!\n");
    }
    return n;
}



char readc(sockfd fd) {
    static ssize_t nread = 0;
    static ssize_t n = 0;
    
    
    if (nread == n) {
        nread = 0;
        n = mread(fd, res.tmpbuf, SOCK_BUF_SIZE);
        //fprintf(stderr, "--bytes: %lu--\n", n);
        if (!n) {
            printf("Remote side closed connection. Exit..\n");
            exit(1);
        }
        if (n < 0) {
            exit(1);
        }
    }

    return res.tmpbuf[nread++];
}

char *readl(sockfd fd) {
    int n;
    char c;
    
    n = 0;
    while ((c = readc(fd)) != '\n') {
        if (c == '\r') continue;
        res.readbuf[n] = c;
        n++;
    }
    res.readbuf[n] = '\0';
    
    return res.readbuf;
}

void handle_ml(sockfd fd, char *s, int code) {
    // MULTILINE !!!!
    // im not sure what we should expect from multiline, so now i just print it
    int tcode;
    char *tbuf;

    tcode = 0;
    tbuf = NULL;
    printf("%s\n", s);
    
    do {
        tbuf = readl(fd);
        printf("%s\n", tbuf);
        if (is_multi_end(tbuf))
            tcode = get_code(tbuf);
    } while (tcode != code);
    
}


ssize_t readln(sockfd fd, char *buf) {
    // Clean buf str is not required, cause mread will rewrite it from start
    //memset(buf, 0, 1);
    
    // tmp buf we'll use for remained bytes from read
    static char *tmp = NULL;
    if(!tmp) {
        tmp = (char *)malloc(BUF_SIZE);
        tmp[0] = '\0';
        
    }
    
    // if we give NULL as a buf pointer we;ll free allocatated memory
    if (!buf) {
        if (tmp) free(tmp);
        return 0;
    }
    
    
    
    char rbuf[SOCK_BUF_SIZE];
    
    ssize_t rn = 0;
    ssize_t buf_i = 0;
    ssize_t tmp_i = 0;
    bool buf_ready = false;
    
    //first we need to get bytes from tmp buffer, if they are exists
    //ssize_t remain_bytes_in_tmp = strlen(tmp);
    
    for (int i = 0; i < strlen(tmp); i++) {
        if (!buf_ready) {
            if (tmp[i] == '\r')
            {} else if (tmp[i] == '\n') {
                buf[buf_i] = '\0';
                buf_ready = true;
            } else {
                buf[buf_i] = tmp[i];
                buf_i++;
            }
        } else  {
            tmp[tmp_i] = tmp[i];
            tmp_i++;
        }
    }
    
    if (buf_ready)
        return strlen(buf);
    
    // in a firstr attemt we need to account of remained bytes in our tmp buffer
    // so we just start to fill buf from rem bytes, and aug num of readed bytes :
    //tmp_n += remain_bytes_in_tmp;
    do {
        
        rn = mread(fd, rbuf, SOCK_BUF_SIZE);
        
        
        // then we work with rbuf which we'd read from network
        for (int i = 0; i < rn; ++i) {
            if (!buf_ready) { // here we check if buf is ready, and remains to handle remained bytes
                if (rbuf[i] == '\r')
                {} else if (rbuf[i] == '\n') {
                    buf[buf_i] = '\0';
                    buf_ready = true;
                } else {
                    buf[buf_i] = rbuf[i];
                    //fprintf(stderr, "%c:%d\n", buf[buf_i], buf[buf_i]);
                    buf_i++;
                }
            } else  {
                tmp[tmp_i] = rbuf[i];
                tmp_i++;
            }
        }
        
    } while (!buf_ready && rn > 0);
    
    return rn;
}

char * addc(char* dst, char c) {  // we should be  sure that  there enough space
    ssize_t len = strlen(dst);
    memset(dst + len, c, 1);
    memset(dst + len + 1, 0, 1);
    return dst;
}

ssize_t readsock(int fd, char *buf, int bs) {
    ssize_t n;
    do {
        n =  read(fd, (void *)buf, bs);
        
    } while ( n < 0 && (errno == EINTR || errno == EAGAIN));
    
    if (n < 0) {
        perror("Error while read from socket.\nQuiting...");
        exit(1);
    }
    return n;
}




ssize_t writesock(int fd, char *buf, ssize_t bs) {
    ssize_t n;
    do {
        n = write(fd, buf, bs);
    } while (n < 0 && (errno == EINTR || errno ==  EAGAIN));
    
    if (n < 0) {
        perror("Error while write to socket");
        exit(1);
    }
    return n;
}

char * assemble_line(char *dst, char *src, ssize_t n) {
    
    char *tmp = NULL;
    for (int i = 0; i < n; i++) {
        if (src[i] != '\r' && src[i] != '\n')
            addc(dst, src[i]);
        else if (src[i] == '\r')
            addc(dst, ' ');
        else {
            addc(dst, '\0');
            tmp = (char *)malloc(strlen(dst) + 1);
            tmp[0] = '\0';
            strcpy(tmp, dst);
            dst[0] = '\0';
        }
    }
    return tmp;
}

ssize_t increase_strbuf_size(char **str, ssize_t size) { //wil return new size of the string
    ssize_t newlen = size * 2;
    
    char *tmp = (char *)malloc(newlen);
    if (!tmp) {
        perror("allocating memory error");
        exit(1);
    }
    tmp[0] = '\0';
    strcpy(tmp, *str);
    free(*str);
    *str = tmp;
    return newlen;
}


ssize_t send_cmd(sockfd fd, char *cmd, char *arg, char *buf) {
    ssize_t n;
    
    if (arg)
        mstrcat(cmd, arg, buf);
    else
        mstrcat_short(cmd, buf);
    
    n = mwrite(fd, buf, strlen(buf));
    
    return n;
}
