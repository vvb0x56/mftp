//
//  hashmap.c
//  mftp
//
//  Created by mac on 30/11/2018.
//  Copyright © 2018 vladimirburlakov. All rights reserved.
//

#include "hashmap.h"

unsigned long
hash(char *str)
{
    unsigned long hash = 5381;
    int c;
    
    while ((c = *str++)){
        hash = ((hash << 5 ) + hash) + c;
    }
    return hash;
}

void
add_to_hashmap(struct hashnode *table[], int size, char *str, int code)
{
    int i = (int)(hash(str) % size);
    
    struct hashnode *newnode = (struct hashnode *)malloc(sizeof(struct hashnode));
    newnode->str = (char *)malloc(strlen(str) + 1);
    strcpy(newnode->str, str);
    newnode->code = code;
    newnode->next = NULL;
    
    if ( !table[i] ) {
        table[i] = newnode;
        return;
    }
    
    struct hashnode *node = table[i];
    while(node->next) {
        node = node->next;
    }
    
    node->next = newnode;
}

void
print_hashmap(struct hashnode *table[], int size)
{
    for (int i = 0; i < size; i++) {
        struct hashnode *node = table[i];
        if (node) {
            do {
                printf("%s:%d; ", node->str, node->code);
                node = node->next;
            } while (node);
        }
        else printf(" - ");
        printf("\n");
    }
}

void
clean_hashmap(struct hashnode *table[], int size)
{
    struct hashnode *tmp = NULL;
    
    for (int i = 0; i < size; i++) {
        struct hashnode *node = table[i];
        while (node) {
            tmp = node->next;
            free(node->str);
            free(node);
            node = tmp;
        }
    }
}

int
get_from_hashmap(struct hashnode *table[], int size, char *str)
{
    int i = (int)(hash(str) % size);
    
    struct hashnode *node = table[i];
    while (node) {
        if (strcmp(node->str, str) == 0)
            return node->code;
        node = node->next;
    }
    
    return -1;
}



