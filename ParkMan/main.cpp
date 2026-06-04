#include <cstdio>
#include <iostream>
#include <cerrno>
#include <filesystem>
#include <fstream>
#include <string>
#include "CommonData.h"
#include <cmath>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <queue>
#include <mqueue.h>


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
#include "Errors.hpp"



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


/*----------------------------------------------------------------------------------------------------------------------*/
/*  Main function from which the program starts running.                                                                */
int main(int const argc, char const *argv[])
 {
  char PIDFileName[PATH_LEN];
  // key_t sh_mem_key;
  // key_t sh_sem_key;

  GPS_Cords_s TelAviv = {32.0853, 34.7818}, Jerusalem = {31.7683, 35.2137};
  double d;
  d = GetDistance(TelAviv, Jerusalem);
  //printf("%f\n\r",d);
  //std::cout << std::fixed << std::setprecision(6) << d << "\n\r";
  std::cout << std::fixed << std::setprecision(6);
  std::cout << d << "\n\r";
 
  own_pid = getpid();

  std::cout << "The current protram is: "<< argv[0] <<"\n\r";
  std::cout << "The pid is: " << own_pid << "\n\r";
  //printf("The current protram is: %s \n\r", argv[0]);
  //printf("The pid is: %d\n\r", own_pid);

  // perror("Testing errors.");
  // std::cerr << "Testing errors." << ": " << std::strerror(errno) << "\n";
  // perr()<<"Testing errors.";

  CreatePIDFile(argc, argv, PIDFileName);
  Enablesignals();


  database_pid = fork();
  switch(database_pid)
   {
    case -1:
      //perror("fork error.");
      perr() << "fork error.";
      exit(EXIT_FAILURE);
     break;
    case 0:
      //printf("Starting DataBase process\n\r");
      std::cout << "Starting DataBase process\n\r";
      DataBaseProc();
      //printf("Finishing DataBase process\n\r");
      std::cout << "Finishing DataBase process\n\r";
      exit(EXIT_SUCCESS);
     break;
    default:
      //printf("The DataBase process started with PID: %d\n\r", database_pid);
      std::cout << "The DataBase process started with PID: "<< database_pid <<"\n\r";
     break;
   }
   
  parking_pid = fork();
  switch(parking_pid)
   {
    case -1:
      //perror("fork error.");
      perr()<<"fork error.";
      exit(EXIT_FAILURE);
     break;
    case 0:
      //printf("Starting Parking process\n\r");
      std::cout << "Starting Parking process\n\r";
      DataBaseProc();
      //printf("Finishing Parking process\n\r");
      std::cout << "Finishing Parking process\n\r";
      exit(EXIT_SUCCESS);
     break;
    default:
      //printf("The Parking process started with PID: %d\n\r", parking_pid);
      std::cout << "The Parking process started with PID: " << parking_pid << "\n\r";
     break;
   }


  network_pid = fork();
  switch(network_pid)
   {
    case -1:
      //perror("fork error.");
      perr() << "fork error.";
      exit(EXIT_FAILURE);
     break;
    case 0:
      //printf("Starting Network process\n\r");
      std::cout << "Starting Network process\n\r";
      NetworkProc();
      //printf("Finishing Network process\n\r");
      std::cout << "Finishing Network process\n\r";
      exit(EXIT_SUCCESS);
     break;
    default:
      //printf("The Network process started with PID: %d\n\r", network_pid);
      std::cout << "The Network process started with PID: " << network_pid << "\n\r";
     break;
   }


//  sleep(10);


 //printf("The loop is infinite. So press Ctrl+C to quit.\n\r");
 std::cout << "The loop is infinite. So press Ctrl+C to quit.\n\r";
  do
  {
     /* code */
 
  } while (1);
  
  WaitUntilFinised();

  // printf("The loop is infinite. So press Ctrl+C to quit.\n\r");
  // do
  // {
  //    /* code */
 
  // } while (1);

  RemovePIDFile(PIDFileName);

  //printf("The program finished running.\n\r");
  std::cout << "The program finished running.\n\r";
  return 0;
 }


void WaitUntilFinised()
 {
  int w;
  int wstatus;
  //printf("Waiting for processes to be stopped...\n\r");
  std::cout << "Waiting for processes to be stopped...\n\r";
  w = waitpid(database_pid, &wstatus, WUNTRACED );  // WUNTRACED | WCONTINUED
  if(w < 0)
   {
    //perror("Was problem in wainting for the DataBase process.");
    perr() << "Was problem in wainting for the DataBase process.";
   }
  w = waitpid(parking_pid, &wstatus, WUNTRACED );  // WUNTRACED | WCONTINUED
  if(w < 0)
   {
    //perror("Was problem in wainting for the Parking process.");
    perr() << "Was problem in wainting for the Parking process.";
   }

  w = waitpid(network_pid, &wstatus, WUNTRACED );  // WUNTRACED | WCONTINUED
  if(w < 0)
   {
    //perror("Was problem in wainting for the Network process.");
    perr() << "Was problem in wainting for the Network process.";
   }

 }


void CreatePIDFile(int const argc, char const *argv[], char FileName[])
 {
  int pid;
  GetPIDFile(argc, argv, FileName);
  pid = getpid();
  //FILE *f;

  std::ofstream outFile(FileName);
  if(outFile.is_open())
   {
    outFile<<pid;
    outFile.close();
   }
  std::cout<<FileName;

  // f = fopen(FileName, "w");
  // if(f)
  //  {
  //   fprintf(f, "%d", pid);
  //   fclose(f);
  //  }
  // printf("%s\n\r", FileName);
 }

void RemovePIDFile(char FileName[])
 {
  remove(FileName);
 }


 
// Custom callback executed when signal arrives
void SignalHandler(int sig) 
 {
  if (sig == DB_UPADATE_SIGNAL)  // Database update signal
   {
    //printf("\nDB_UPADATE_SIGNAL signal was received successfully. \n");
    std::cout << "\nDB_UPADATE_SIGNAL signal was received successfully. \n";
   }
  if (sig == SIGINT)  // Ctrl-C Signal
   {
    //printf("\nSIGINT signal was received successfully. \n");
    std::cout << "\nSIGINT signal was received successfully. \n";
   }
  
  if (sig == SIGQUIT)  // Ctrl-\ Signal
   {
    //printf("\nSIGQUIT signal was received successfully. \n");
    std::cout << "\nSIGQUIT signal was received successfully. \n";
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
  // Bind Ctrl-C signal to our handler function
  sigaction(SIGINT, &sa, NULL);
  // Bind Ctrl-\ signal to our handler function
  sigaction(SIGQUIT, &sa, NULL);
  
  //printf("The signals were created successfully.\n\r");
  std::cout << "The signals were created successfully.\n\r";
 }



