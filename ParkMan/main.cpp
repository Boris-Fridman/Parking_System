#include <iostream>
#include "CommonData.h"
#include <cmath>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>

#include "DataBase.hpp"
#include "Network.hpp"
#include "Parking.hpp"



void WaitUntilFinised();

/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Main Function from which the program starts running. 
 * *************************************************************************************************************
 */
pid_t database_pid;
pid_t parking_pid;
pid_t network_pid;

/*----------------------------------------------------------------------------------------------------------------------*/
/*  Main function from which the program starts running.                                                                */
int main()
 {
  
  GPS_Cords_s TelAviv = {32.0853, 34.7818}, Jerusalem = {31.7683, 35.2137};
  double d;
  d = GetDistance(TelAviv, Jerusalem);
  printf("%f\n\r",d);
 
  
  database_pid = fork();
  switch(database_pid)
   {
    case -1:
      perror("fork error.");
      exit(EXIT_FAILURE);
     break;
    case 0:
      printf("Starting DataBase process\n\r");
      DataBaseProc();
      printf("Finishing DataBase process\n\r");
      exit(EXIT_SUCCESS);
     break;
    default:
      printf("The DataBase process started with PID: %d\n\r", database_pid);
     break;
   }
   
  parking_pid = fork();
  switch(parking_pid)
   {
    case -1:
      perror("fork error.");
      exit(EXIT_FAILURE);
     break;
    case 0:
      printf("Starting Parking process\n\r");
      DataBaseProc();
      printf("Finishing Parking process\n\r");
      exit(EXIT_SUCCESS);
     break;
    default:
      printf("The Parking process started with PID: %d\n\r", parking_pid);
     break;
   }


  network_pid = fork();
  switch(network_pid)
   {
    case -1:
      perror("fork error.");
      exit(EXIT_FAILURE);
     break;
    case 0:
      printf("Starting Network process\n\r");
      NetworkProc();
      printf("Finishing Network process\n\r");
      exit(EXIT_SUCCESS);
     break;
    default:
      printf("The Network process started with PID: %d\n\r", network_pid);
     break;
   }


  sleep(10);


  printf("The loop is infinite. So press Ctrl+C to quit.\n\r");
//   do
//   {
//      /* code */
 
//   } while (1);
  
  WaitUntilFinised();
  printf("The program finished running.\n\r");
  return 0;
 }


void WaitUntilFinised()
 {
  int w;
  int wstatus;
  printf("Waiting for processes to be stopped...\n\r");
  w = waitpid(database_pid, &wstatus, WUNTRACED );  // WUNTRACED | WCONTINUED
  if(w < 0)
   {
    perror("Was problem in wainting for the DataBase process.");
   }
  w = waitpid(parking_pid, &wstatus, WUNTRACED );  // WUNTRACED | WCONTINUED
  if(w < 0)
   {
    perror("Was problem in wainting for the Parking process.");
   }

  w = waitpid(network_pid, &wstatus, WUNTRACED );  // WUNTRACED | WCONTINUED
  if(w < 0)
   {
    perror("Was problem in wainting for the Network process.");
   }

 }