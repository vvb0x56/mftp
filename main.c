//
//  main.c
//  mftp
//
//  Created by mac on 26/11/2018.
//  Copyright © 2018 vladimirburlakov. All rights reserved.
//



#include "common.h"
#include "str_func.h"
#include "conn.h"
#include "hashmap.h"
#include "do_all.h"
#include "help.h"

enum {
    HELP = 0,
    DELE,       /*expected code - 250    delete file*/
    RMD,        /*250    delete dir*/
    CWD,        /*250    change dir*/
    MKD,        /*257    create dir*/
    PWD,        /*257    current dir*/
    QUIT,       /*221    quit*/
    TYPE,       /*200    set type of connection*/
    PORT,       /*200    active mode*/
    PASV,       /*227    passv mode*/
    LIST,       /*150,226    ls*/
    RETR,       /*150,226    get file*/
    STOR,       /*150,226    put file*/
    ABOR,       /*426,226    cancel transfer*/
    RNFR,       /*350    choose file for rename*/
    RNTO       /*250    new name*/
};

const int BUF_SIZE = 4096;
const int SOCK_BUF_SIZE = 1024;
const int MAX_PASSW_LEN = 1024;
const int CMD_SIZE = 4;
const int HASH_TABLE_SIZE = 32;
const int MAX_USER_INPUT_ELEM_NUM = 3;

resources res;

void clear_res (resources *r, sockfd fd) {
    readln(0, NULL);
    clean_hashmap(r->ht, HASH_TABLE_SIZE);
    free(r->inputbuf);
    free(r->sendbuf);
    free(r->readbuf);
    free(r->pwdbuf);
    free(r->tmpbuf);
}


int
main(int argc, char** argv)
{
    
    
    
    if (argc < 2) {
        printf("Usage:\n %s IPADDR\n\n", argv[0]);
        return 1;
    }
    
    //// hashmap filling : 

    
    struct hashnode *hash_table[HASH_TABLE_SIZE];
    memset(hash_table, 0, sizeof(struct hashnode *) * HASH_TABLE_SIZE);
    res.ht = hash_table;
    
    add_to_hashmap(res.ht, HASH_TABLE_SIZE, "help", HELP);
    add_to_hashmap(res.ht, HASH_TABLE_SIZE, "rm", DELE);
    add_to_hashmap(res.ht, HASH_TABLE_SIZE, "rmdir", RMD);
    add_to_hashmap(res.ht, HASH_TABLE_SIZE, "cd", CWD);
    add_to_hashmap(res.ht, HASH_TABLE_SIZE, "mkdir", MKD);
    add_to_hashmap(res.ht, HASH_TABLE_SIZE, "pwd", PWD);
    add_to_hashmap(res.ht, HASH_TABLE_SIZE, "quit", QUIT);
    add_to_hashmap(res.ht, HASH_TABLE_SIZE, "exit", QUIT);
    add_to_hashmap(res.ht, HASH_TABLE_SIZE, "type", TYPE);
    add_to_hashmap(res.ht, HASH_TABLE_SIZE, "acti", PORT);
    add_to_hashmap(res.ht, HASH_TABLE_SIZE, "pasv", PASV);
    add_to_hashmap(res.ht, HASH_TABLE_SIZE, "ls", LIST);
    add_to_hashmap(res.ht, HASH_TABLE_SIZE, "get", RETR);
    add_to_hashmap(res.ht, HASH_TABLE_SIZE, "put", STOR);
    add_to_hashmap(res.ht, HASH_TABLE_SIZE, "cancel", ABOR);
    add_to_hashmap(res.ht, HASH_TABLE_SIZE, "mv", RNFR);
    add_to_hashmap(res.ht, HASH_TABLE_SIZE, "to", RNTO);
   
    
    
    sockfd  ccfd;   // cfd - control chachel fd
    bool logged;
    
    
   

    res.inputbuf = (char *)malloc(BUF_SIZE); //console/user input  buf
    res.sendbuf = (char *)malloc(BUF_SIZE);
    res.readbuf = (char *)malloc(BUF_SIZE);
    res.pwdbuf = (char *)malloc(BUF_SIZE);
    res.tmpbuf = (char *)malloc(SOCK_BUF_SIZE);

    
    if (!res.inputbuf || !res.sendbuf
        || !res.readbuf || !res.pwdbuf
        || !res.tmpbuf) {
        perror("Error to allocate memory for bufs in main()\n");
        exit(1);
    }
    
    ccfd = get_sock(argv[1], 21);
    
    
    logged = do_auth(ccfd);
    
    if (!logged) {
        printf("Login incorrect. Exceeded number of available attempts.\n");
        close(ccfd);
        return 1;
    }
    
    int code;
    
    char* user_input_args[MAX_USER_INPUT_ELEM_NUM];
    char* user_cmd;
    char* user_arg1;
    char* user_arg2;
   
   
    // MAIN LOOP
    
    while(1) {
        do_pwd(ccfd, res.sendbuf, res.readbuf, res.pwdbuf);
        printf("%s > ", res.pwdbuf);
        
        
        if (!fgets(res.inputbuf, BUF_SIZE, stdin)) {
            if (feof(stdin)) {
                printf("\nLogout..\n");
                exit(1);
            }
        }
        mstrtok(res.inputbuf, user_input_args);
        user_cmd = user_input_args[0];
        user_arg1 = user_input_args[1];
        user_arg2 = user_input_args[2];
        

        if (user_cmd){
            code = get_from_hashmap(res.ht, HASH_TABLE_SIZE, user_cmd);
            switch (code) {
                case HELP:
                    print_help();
                    break;
                case DELE:
                    if (user_arg1)
                        do_dele(ccfd, user_arg1, res.sendbuf, res.readbuf);
                    break;
                case RMD:
                     if (user_arg1)
                         do_rmd(ccfd, user_arg1, res.sendbuf, res.readbuf);
                    break;
                case MKD:
                    if (user_arg1)
                        do_mkd(ccfd, user_arg1, res.sendbuf, res.readbuf);
                    break;
                case PWD:
                    do_pwd(ccfd, res.sendbuf, res.readbuf, res.pwdbuf);
                    printf("%s\n", res.pwdbuf);
                    break;
                case CWD:
                    if (user_arg1)
                        do_cwd(ccfd, user_arg1, res.sendbuf, res.readbuf);
                    break;
                case LIST:
                    do_list(ccfd, user_arg1, res.sendbuf, res.readbuf);
                    break;
                case RETR:
                    if (user_arg1)
                        do_get(ccfd, user_arg1, res.sendbuf, res.readbuf);
                    break;
                case STOR:
                    if (user_arg1) {
                        do_put(ccfd, user_arg1, res.sendbuf, res.readbuf);
                    }
                    
                    break;
                case RNFR:
                    if(user_arg1 || user_arg2) {
                        do_mv(ccfd, user_arg1, user_arg2, res.sendbuf, res.readbuf);
                    }
                    break;
                case QUIT:
                    clear_res (&res, ccfd);
                    send_cmd(ccfd, "QUIT", NULL, res.sendbuf);
                    printf("Exit..\n");
                    close(ccfd);
                    return 0;
                    break;
                default:
                    printf(" Unknow command.\n");
                    break;
            }
        }
    }
    
    return 0;
}

// 220 service ready for new user
