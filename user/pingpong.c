/*
Write a program that uses UNIX system calls to ''ping-pong'' 
a byte between two processes over a pair of pipes, one for each direction. 
The parent should send a byte to the child; the child should print "<pid>: received ping", 
where <pid> is its process ID, write the byte on the pipe to the parent, and exit; 
the parent should read the byte from the child, print "<pid>: received pong", and exit.
Your solution should be in the file user/pingpong.c.

Use pipe to create a pipe.
Use fork to create a child.
Use read to read from the pipe, and write to write to the pipe.
Use getpid to find the process ID of the calling process.
Add the program to UPROGS in Makefile.
User programs on xv6 have a limited set of library functions available to them. You can see the list in user/user.h; the source (other than for system calls) is in user/ulib.c, user/printf.c, and user/umalloc.c.
*/
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[]){
    int p[2];
    pipe(p);
    int pid = fork();
    if(pid == 0){
        char childReadBuff;
		if (read(p[0], &childReadBuff, 1) != 1)
		{
			fprintf(2, "failed to read in child\n");
			exit(1);
		}
        close(p[0]);
        printf("%d: received ping\n", getpid());
		if(write(p[1], &childReadBuff, 1) != 1)
		{
			fprintf(2, "failed to write in child\n");
			exit(1);
		}
        close(p[1]);
    }
    else{
        char *buf = "a";
        char parentReadBuff[2];
        if (write(p[1], buf, 1) != 1){
            fprintf(2, "Can't write to child in parent!\n");
            exit(1);
        }
        close(p[1]);
        wait(0);
        if (read(p[0], parentReadBuff, 1) != 1){
            fprintf(2, "Can't read from child in parent!");
            exit(1);
        }
        printf("%d: received pong\n", getpid());
        close(p[0]);
        exit(0);  
    }
}