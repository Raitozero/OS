/*
Write a concurrent version of prime sieve using pipes. 
This idea is due to Doug McIlroy, inventor of Unix pipes. 
The picture halfway down this page and the surrounding text explain how to do it. (https://swtch.com/~rsc/thread/)
Your solution should be in the file user/primes.c.

Be careful to close file descriptors that a process doesn't need, because otherwise your program will run xv6 out of resources before the first process reaches 35.
Once the first process reaches 35, it should wait until the entire pipeline terminates, including all children, grandchildren, &c. Thus the main primes process should only exit after all the output has been printed, and after all the other primes processes have exited.
Hint: read returns zero when the write-side of a pipe is closed.
It's simplest to directly write 32-bit (4-byte) ints to the pipes, rather than using formatted ASCII I/O.
You should create the processes in the pipeline only as they are needed.
Add the program to UPROGS in Makefile.
*/
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void prime(int p[2]){
    int curr, rest;
    close(p[1]);
    int isNotEnd = read(p[0], &curr, 4);
    if(!isNotEnd){
        exit(0);
    }
    else if(isNotEnd != 4){
        fprintf(2, "Read failed.\n");
        exit(1);
    }
    printf("prime %d\n", curr);
    int pp[2];
    pipe(pp);
    int pid = fork();
    if(pid == 0){
        prime(pp);
    }
    else{
        close(pp[0]);
        while(read(p[0], &rest, 4)){
            if(rest % curr != 0){
                if(write(pp[1], &rest, 4) != 4){
                    printf("Writing to child failed.\n");
                };
            }
        }
        close(p[0]);
        close(pp[1]);
        wait(0);
    }
}

int
main(int argc, char *argv[]){
    int p[2];
    pipe(p);
    int pid = fork();
    if(pid == 0){
        prime(p);
    }
    else{
        close(p[0]);
        for(int i = 2; i <= 35; i++){
            if(write(p[1],&i,4)!=4){
                fprintf(2, "Initial writing failed\n");
                exit(1);
            }
        }
        close(p[1]);
        wait(0);
        exit(0);
    }
}