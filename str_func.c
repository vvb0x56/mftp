//
//  str_func.c
//  mftp
//
//  Created by mac on 17/12/2018.
//  Copyright © 2018 vladimirburlakov. All rights reserved.
//

#include "str_func.h"

extern const int MAX_USER_INPUT_ELEM_NUM;

void
mstrcat(char *cmd, char* arg, char *buf) {
    buf[0] = '\0';
    strcat(buf, cmd);
    strcat(buf, " ");
    strcat(buf, arg);
    strcat(buf, "\r\n");
}

void
mstrcat_short(char *cmd, char *buf) {
    buf[0] = '\0';
    strcat(buf, cmd);
    strcat(buf, "\r\n");
}



bool
is_multi_beg(char *str) {
    if (!str)
        return false;
    ssize_t len = strlen(str);
    if (len < 5)
        return false;
    
    return str[3] == '-' ? true : false;
}

bool
is_multi_end(char *str) {
    if (!str)
        return false;
    ssize_t len = strlen(str);
    if (len < 5)
        return false;
    return str[3] == ' ' ? true : false;
}



char *
parse_pwd(char *line) {
    // we just want to get second part of \" token
    char *tmp;
    tmp = strtok(line, "\"");
    if (tmp)
        tmp = strtok(NULL, "\"");
    if (tmp)
        return tmp;
    return NULL;
}



void
cmdcat2(char *cmd, char* op, char *buf) {
    strcat(buf, cmd);
    strcat(buf, " ");
    strcat(buf, op);
    strcat(buf, "\r\n");
}



void
mstrtok(char *src, char *buf[]) {
    // this function will make from string : "  cmd file\ 1.txt  file\ 2.txt file\ 3.txt"
    // this string: "cmd\0file 1.txt\0file 2.txt\0file 3.txt\03.txt"
    // and tried to fill buf to contain pointers to each token
    // but not more that MAX_USER_INPUT_ELEM_NUM
    
    memset(buf, 0, MAX_USER_INPUT_ELEM_NUM * sizeof(char *));
    
    bool intoken = false; //token
    int ntc = 0; // new token count
    bool ent = true; // expect new token
    char *iptr = src; // insert pointer
    while (*src) {
        if (*src != ' ' && *src != '\\' && *src != '\n') {
            *iptr = *src;
            
            if (ent) {
                if (ntc < MAX_USER_INPUT_ELEM_NUM) {
                    buf[ntc] = iptr;
                    ntc++;
                }
                ent = false;
            }
            
            ++iptr;
            intoken = true;
        } else if (*src == '\\') {
            src++;
            if (*src) {
                *iptr = *src;
                ++iptr;
            } else break;
        } else {
            if (intoken) {
                *iptr = '\0';
                iptr++;
                intoken = false;
                ent = true;
            }
        }
        ++src;
    }
    *iptr = '\0';
}

void handle_error_code_4xx_5xx(int code,  char *s) {
    switch (code) {
        case 421:
            printf("Service not available, closing control connection.\n");
            printf("Exit..\n");
            exit(1);
            break;
        case 425:
            printf("Can't open data connection.\n");
            break;
        case 426:
            printf("Connection closed; transfer aborted.\n");
            break;
        case 450:
            printf("Requested file action not taken. File unavailable (e.g., file busy).\n");
            break;
        case 451:
            printf("Requested action aborted: local error in processing.\n");
            break;
        case 500:
            printf("Syntax error, command unrecognized.\n");
            break;
        case 501:
            printf("Syntax error in parameters or arguments.\n");
            break;
        case 502:
            printf("Command not implemented.\n");
            break;
        case 503:
            printf("Bad sequence of commands.\n");
            break;
        case 530:
            printf("Not logged in.\n");
            printf("Exit..\n");
            exit(1);
            break;
        case 532:
            printf("Need account for storing files.\n");
            break;
        case 550:
            printf("Requested action not taken. File unavailable (e.g., not found, no access).\n");
            break;
        case 551:
            printf("Requested action aborted: page type unknown.\n");
            break;
        case 552:
            printf("Requested file action aborted. Exceeded storage allocation\n");
            break;
        case 553:
            printf("Requested action not taken. File name not allowed.\n");
            break;
        default:
            printf("Unknown command: %s\n", s);
            break;
            
    }
}

int
get_code(char *str) {
    if (!str)
        return 0;
    
    ssize_t len =  strlen(str);
    if (len < 4)  // 3-digits +  closing \0
        return 0;
    
    // if (str[3] != ' ' || str[3] != '-')
    //     return 0;
    
    char code_str[4]; // 3-digits +  closing \0
    memcpy(code_str, str, 3); // get the  digits
    code_str[3] = '\0';
    
    return atoi(code_str);
}

void
pwd_parse(char *src, char *dst) {
    // we just want to get second part of \" token
    dst[0] = '\0';
    char *tmp = strtok(src, "\"");
    if (tmp)
        tmp = strtok(NULL, "\"");
    if (tmp)
        strcpy(dst, tmp);
}


void
cmdcat(char *cmd, char *input) {
    char tmp[1024];
    tmp[0] ='\0';
    strcat(tmp, cmd);
    strcat(tmp, " ");
    strcat(tmp, input);
    
    strcpy(input, tmp);
}

void clean_r_n(char *s) {
    int i;
    for (i = 0; i < strlen(s); ++i)
         if (s[i] == '\r' || s[i] == '\n')
             s[i] = '\0';
}
