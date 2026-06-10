#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "CommonData.h"

#include "Network.h"
#include "I2C.h"


typedef void(*process)(void);

pid_t OpenProcess(void(*process)(void), char ProcName[])
 {
  pid_t proc_pid;
  proc_pid = fork();
  switch(proc_pid)
   {
    case -1:
      perror("fork error.");
      exit(EXIT_FAILURE);
     break;
    case 0:
      printf("Starting new process\n\r");
      process();
      exit(EXIT_SUCCESS);
     break;
    default:
      printf("The new %s process with PID: %d started.\n\r", ProcName, proc_pid);
      return proc_pid;
     break;
   }
 }


int main(void)
 {
  pid_t network_pid;
  pid_t i2c_pid;
  UNUSED(network_pid);
  UNUSED(i2c_pid);
  //ansi clear screen
  printf("\033[2J\033[H");
  
  network_pid = OpenProcess(NetworkProc, "Network");

  i2c_pid = OpenProcess(I2CProc, "I2C");

  //code
  //NetworkProc();  // Temporary for test is run from main. Later will be redefined as a process.
  while(1)
   {
   }
  return 0;
 }