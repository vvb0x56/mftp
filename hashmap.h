//
//  hashmap.h
//  mftp
//
//  Created by mac on 30/11/2018.
//  Copyright © 2018 vladimirburlakov. All rights reserved.
//
//Hash map function for string was gotten here :
//http://www.cse.yorku.ca/~oz/hash.html

#ifndef hashmap_h
#define hashmap_h

#include "common.h"

struct hashnode
{
    char *str;
    int code;
    struct hashnode *next;
};

unsigned long
hash(char *str);

void
add_to_hashmap(struct hashnode *table[], int size, char *str, int code);

void
print_hashmap(struct hashnode *table[], int size);

void
clean_hashmap(struct hashnode *table[], int size);

int
get_from_hashmap(struct hashnode *table[], int size, char *str);


#endif /* hashmap_h */
