#include "kernel/types.h"
#include "user/user.h"


int
main(int argc, char *argv[])
{

  if(argc <= 1){
    fprintf(2, "Error: argument is expected\n");
    exit(1);
  }

  char* ticks_string;
  ticks_string = argv[1];
  int ticks;
  ticks = atoi(ticks_string);
  sleep(ticks);
  exit(0);
}