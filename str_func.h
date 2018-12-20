//
//  str_func.h
//  mftp
//
//  Created by mac on 17/12/2018.
//  Copyright © 2018 vladimirburlakov. All rights reserved.
//

#ifndef str_func_h
#define str_func_h

#include "common.h"

void mstrcat(char *cmd, char* arg, char *buf);

void mstrcat_short(char *cmd, char *buf);

bool is_multi_beg(char *str);

bool is_multi_end(char *str);

char * parse_pwd(char *line);

void cmdcat2(char *cmd, char* op, char *buf);

void mstrtok(char *src, char *buf[]);

void handle_error_code_4xx_5xx(int code,  char *s);

int get_code(char *str);

void pwd_parse(char *src, char *dst);

void cmdcat(char *cmd, char *input);

char * addc(char* dst, char c);

void clean_r_n(char *s);


#endif /* str_func_h */
