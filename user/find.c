/*
Write a simple version of the UNIX find program: find all the files in a directory tree with a specific name. 
Your solution should be in the file user/find.c.

Look at user/ls.c to see how to read directories.
Use recursion to allow find to descend into sub-directories.
Don't recurse into "." and "..".
Changes to the file system persist across runs of qemu; to get a clean file system run make clean and then make qemu.
You'll need to use C strings. Have a look at K&R (the C book), for example Section 5.5.
Note that == does not compare strings like in Python. Use strcmp() instead.
Add the program to UPROGS in Makefile.
*/
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*
fmtname(char *path)
{
    static char buf[DIRSIZ+1];
    char *p;

    // Find first character after last slash.
    for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
    p++;

    // Return blank-padded name.
    if(strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
    return buf;
}


void find(char* path, char* fileName){
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;
    if((fd = open(path, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
    switch(st.type){
    case T_FILE:
        if(strcmp(fmtname(path), fileName) == 0){
            printf("%s\n", path);
        }
        break;
    case T_DIR:
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        //buf = "/abc"
        p = buf+strlen(buf);
        *p++ = '/';
        //buf = "/abc/"
        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            if(de.inum == 0 || !strcmp(de.name, ".") || !strcmp(de.name, "..")) continue;
            memmove(p, de.name, DIRSIZ);
            //buf = "/abc/someName"
            p[DIRSIZ] = 0; //same to p[DIRSIZ] = '\0'; same to *(p + DIRSIZ) = '\0';
            if(stat(buf, &st) < 0){
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            switch(st.type){
            case T_FILE:
            if(strcmp(de.name, fileName) == 0){
                printf("%s\n", buf);
            }
            case T_DIR:
            find(buf, fileName);
            }
        }
        break;
    }
    close(fd);
}


int
main(int argc, char *argv[])
{
    if(argc < 3){
        fprintf(2, "error: arguments too few\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}