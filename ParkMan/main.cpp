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

void CreatePIDFile(int const argc, char const *argv[], char FileName[]);
void RemovePIDFile(char FileName[]);
void Enablesignals();




/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Main Function from which the program starts running. 
 * *************************************************************************************************************
 */
pid_t database_pid;
pid_t parking_pid;
pid_t network_pid;
pid_t own_pid;

char PIDFileName[PATH_LEN];

/*----------------------------------------------------------------------------------------------------------------------*/
/*  Main function from which the program starts running.                                                                */
int main(int const argc, char const *argv[])
 {
  
  GPS_Cords_s TelAviv = {32.0853, 34.7818}, Jerusalem = {31.7683, 35.2137};
  double d;
  d = GetDistance(TelAviv, Jerusalem);
  printf("%f\n\r",d);
 
  own_pid = getpid();
  printf("The current protram is: %s \n\r", argv[0]);
  printf("The pid is: %d\n\r", own_pid);

  CreatePIDFile(argc, argv, PIDFileName);
  Enablesignals();


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


//  printf("The loop is infinite. So press Ctrl+C to quit.\n\r");
//   do
//   {
//      /* code */
 
//   } while (1);
  
  WaitUntilFinised();

  printf("The loop is infinite. So press Ctrl+C to quit.\n\r");
  do
  {
     /* code */
 
  } while (1);

  RemovePIDFile(PIDFileName);

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


void CreatePIDFile(int const argc, char const *argv[], char FileName[])
 {
  int pid;
  GetPIDFile(argc, argv, FileName);
  pid = getpid();
  FILE *f;
  f = fopen(FileName, "w");
  if(f)
   {
    fprintf(f, "%d", pid);
    fclose(f);
   }
  printf("%s\n\r", FileName);
 }

void RemovePIDFile(char FileName[])
 {
  remove(FileName);
 }


 
// Custom callback executed when signal arrives
void SignalHandler(int sig) 
 {
  if (sig == DB_UPADATE_SIGNAL) 
   {
    printf("\nDB_UPADATE_SIGNAL signal was received successfully. \n");
   }
 }

void Enablesignals()
 {
  struct sigaction sa;

  // Configure the sigaction structure
  sa.sa_handler = &SignalHandler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);

  // Bind DB_UPADATE_SIGNAL to our handler function
  sigaction(DB_UPADATE_SIGNAL, &sa, NULL);
  printf("The signals were created successfully.\n\r");
 }



