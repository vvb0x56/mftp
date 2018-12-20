//
//  help.h
//  mftp
//
//  Created by mac on 14/12/2018.
//  Copyright © 2018 vladimirburlakov. All rights reserved.
//

#ifndef help_h
#define help_h

void
print_help() {
    printf("Available commands: \n");
    printf("    help    -- this help message\n");
    printf("    rm      -- delete file\n");
    printf("    rmdir   -- delete directory\n");
    printf("    mkdir   -- create directory\n");
    printf("    get     -- download file\n");
    printf("    put     -- upload file\n");
    printf("    mv      -- move file/directory\n");
    printf("    cd      -- change directory\n");
    printf("    pwd     -- return working directory name\n");
    printf("    ls      -- list files in directory\n");
    printf("    quit/exit   -- exit\n");
}


#endif /* help_h */
