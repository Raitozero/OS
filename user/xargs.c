/*
Write a simple version of the UNIX xargs program: read lines from the standard input and run a command for each line, 
supplying the line as arguments to the command. Your solution should be in the file user/xargs.c.

Use fork and exec to invoke the command on each line of input. Use wait in the parent to wait for the child to complete the command.
To read individual lines of input, read a character at a time until a newline ('\n') appears.
kernel/param.h declares MAXARG, which may be useful if you need to declare an argv array.
Add the program to UPROGS in Makefile.
Changes to the file system persist across runs of qemu; to get a clean file system run make clean and then make qemu.
*/

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"


// for this command "find . b | xargs grep hello"
//1. the result of "find . b" will be pass in as stdin to "xargs grep hello"
//2. argv[0] = "xargs" argv[1] = "grep" argv[2] = "hello"

int readLine(char** args,  int curr){
    char buf[1024];
    int p = 0;
    while(read(0, buf+p, 1)){
        if(buf[p] == '\n'){
            buf[p] = 0;
            break;
        }
        p++;
        if(p >= 1024){
            fprintf(2,"Exceed Limits!\n");
            exit(1);
        }
    }
    if(!p) return -1;
    int right = 0;
    while(right < p){
        while(right < p && buf[right]==' '){
            right++;
        }
        int left = right;
        while(right < p && buf[right]!=' '){
            right++;
        }
        buf[right++] = 0;
        args[curr] = malloc(strlen(buf+left) + 1);
        strcpy(args[curr++], buf+left);
        //args[curr++] = buf + left;
    }
    return curr;
}


int
main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(2, "Usage: xargs command [args ...]\n");
        exit(1);
    }
    //"find . b | xargs grep hello"
    char* args[MAXARG];
    for(int i = 1; i < argc; i++){
        args[i - 1] = malloc(strlen(argv[i]) + 1);
        strcpy(args[i-1], argv[i]); //args ={"grep", "hello",...}
    }
    int curr;
    while((curr = readLine(args, argc-1)) != -1){
        args[curr] = 0;
        int pid = fork();
        if(pid == 0){
            exec(args[0], args);
            fprintf(2, "exec failed.\n");// will not reach this point if no error
            exit(1);
        }
        else{
            wait(0);
        }
    }
    exit(0);
}