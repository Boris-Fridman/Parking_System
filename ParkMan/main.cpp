#include "main.hpp"

#include <cstdio>
#include <iostream>
#include <cerrno>
#include <filesystem>
#include <fstream>
#include <string>
#include "CommonData.h"
#include <cmath>
#include <cstring>
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
#include "Processes.hpp"

#include "Configuration.hpp"



#define SIGNAL_TO_TERMINATE   SIGINT





void CatchChildZombie(ProcMan_c *TaskControl = nullptr);
void WaitUntilFinised();

void CreatePIDFile(char FileName[], size_t MaxSize);
void RemovePIDFile(char const FileName[]);
void EnableSignals();

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


bool FullExist = false;
bool UpdateDataBase = false;

/*----------------------------------------------------------------------------------------------------------------------*/
/*  Main function from which the program starts running.                                                                */
int main(int const argc, char const *argv[])
 {
  UNUSED(argc);
  UNUSED(argv);
  char OwnName[PATH_LEN];
  char PIDFileName[PATH_LEN];
  GetOwnNamePath(OwnName, sizeof(OwnName));

  if(PrevProcCopyRunning(OwnName, false))  /* Because the program is a server it cannot run in several copies at all neither from the same path nor from different pathes. */
   {
    printf("The program is allready running.\n\r");
    exit(-2);
   }
  printf("Starting ParkMan Program...\n\r");
  
  InitConfiguration(OwnName);

  ProcMan_c TaskContSh;  /* Attention !!! The class must be defined when configuration was allready loaded. */
  TaskContSh.LogEvent(MakeLogMessage(E_LOG_MESSAGE, MAIN_PROC_NAME, "Starting ParkMan Program..."));

  own_pid = getpid();

  std::cout << "The current program is: "<< OwnName <<"\n\r";
  std::cout << "The pid is: " << own_pid << "\n\r";

  {
   char PathFileName[PATH_LEN] = {0};
   TaskContSh.SetDBFileName(GetDataBaseFilePathName());
   
   std::cout << "Loaded DataBase File: " << PathFileName << "\n\r";

   TaskContSh.SetSHPFileName(GetGeoLocShapeFilePathName());
   std::cout << "Loaded Shape File: " << PathFileName << "\n\r";
  }
  
  CreatePIDFile(PIDFileName, sizeof(PIDFileName));
  EnableSignals();

  ProcParams_s ProcParams = {.sh_mem_key = TaskContSh.ShMemKey(), .sem_name = TaskContSh.SemName().c_str(), .sq_name = TaskContSh.QueueName().c_str(), .qsem_name = TaskContSh.QSemName().c_str(), .ProcType = PROC_DATABASE_E};


  ProcParams.ProcType = PROC_DATABASE_E;
  database_pid = OpenProcess(DataBaseProc, ProcParams, (char*)DB_PROC_NAME, &TaskContSh);
   
  ProcParams.ProcType = PROC_PARKING_E;
  parking_pid = OpenProcess(ParkingProc, ProcParams, (char*)PARK_PROC_NAME, &TaskContSh);
  
  ProcParams.ProcType = PROC_NETWORK_E;
  network_pid = OpenProcess(NetworkProc, ProcParams, (char*)NETW_PROC_NAME, &TaskContSh);

 

//  sleep(10);
//  TaskContSh.ExitProcess(PROC_DATABASE_E);
//  sleep(10);
//  TaskContSh.ExitProcess(PROC_NETWORK_E);
//  sleep(10);
//  TaskContSh.ExitProcess(PROC_PARKING_E);
//  sleep(10);

  std::cout << "The loop is infinite. So press Ctrl+C to quit.\n\r";
  do
   {
    /* code */
    if(UpdateDataBase)
     {
      TaskContSh.ReloadDatabase();
      UpdateDataBase = false;
     }
    CatchChildZombie(&TaskContSh);
    sleep(1);
   } while (!FullExist);

  TaskContSh.ExitAllProcesses();

  WaitUntilFinised();

  RemovePIDFile(PIDFileName);  /* Removes file with the main pid of this program. */

  TaskContSh.LogEvent(MakeLogMessage(E_LOG_MESSAGE, MAIN_PROC_NAME, "The ParkMan Program finished running."));

  std::cout << "The program finished running.\n\r";
  return 0;
 }







/* Catching Zombie-Child-Processes and removing them. */ 
void CatchChildZombie(ProcMan_c *TaskControl)
 {
  //bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */

  int wstatus, w = 0;
  do
  {
   w = waitpid(-1, &wstatus, WNOHANG);  // WUNTRACED | WCONTINUED // WNOHANG
   if(w > 0)
    {
     std::cout << "The process with PID: " << (StdOutNoPiping ? PROC_PID_COLOR : "") << w  << (StdOutNoPiping ? TermColorsReset : "") << " finished running.\n\r";
     if(TaskControl != nullptr)
      {
       std::ostringstream stream;    stream.str("");    stream.clear();
       stream << "The process with PID: " << w << " finished running emergencely.";
       TaskControl->LogEvent(MakeLogMessage(E_LOG_FAIL, MAIN_PROC_NAME, stream.str().c_str()));      
      }
    }
  } while (w > 0);
 }

void WaitUntilFinised()
 {
  int w;
  int wstatus;

  std::cout << "Waiting for processes to be stopped...\n\r";

  w = waitpid(database_pid, &wstatus, WUNTRACED );  // WUNTRACED | WCONTINUED
  if(w < 0)
   {
    perr() << "Was problem in wainting for the DataBase process.";
   }
   
  w = waitpid(parking_pid, &wstatus, WUNTRACED );  // WUNTRACED | WCONTINUED
  if(w < 0)
   {
    perr() << "Was problem in wainting for the Parking process.";
   }

  w = waitpid(network_pid, &wstatus, WUNTRACED );  // WUNTRACED | WCONTINUED
  if(w < 0)
   {
    perr() << "Was problem in wainting for the Network process.";
   }

 }

void CreatePIDFile(char FileName[], size_t MaxSize)
 {
  int pid;
  strncpy(FileName, GetProgInfoPIDFilePathName(), MaxSize - 1);
  pid = getpid();
  
  std::ofstream outFile(FileName);
  if(outFile.is_open())
   {
    outFile << pid;
    outFile.close();
   }
  std::cout << FileName << "\n\r";

 }

void RemovePIDFile(char const FileName[])
 {
  remove(FileName);
 }


/* Custom callback executed when signal arrives */
void AdvancedSignalHandler(int sig, siginfo_t *info, void *context) 
 {
  UNUSED(context);
  if (sig == DB_UPADATE_SIGNAL)  /* Database update signal */
   {
    std::cout << "\nDB_UPADATE_SIGNAL signal was received successfully. \n";
    int passed_val = info->si_value.sival_int;  /* Reading value sent with signal from the sending program. */
    int process_pid = info->si_pid;             /* Reading process pid of the signal sending program.       */
    std::cout << "The passed value is: " << passed_val << "  From process id: " << process_pid << "\n\r";
    UpdateDataBase = true;
   }

  if (sig == SIGINT)   /* Ctrl-C Signal */
   {
    std::cout << "\nSIGINT signal was received successfully. \n";
    FullExist = true;
   }
  
  if (sig == SIGQUIT)  /* Ctrl-\ Signal */
   {
    std::cout << "\nSIGQUIT signal was received successfully. \n";
    FullExist = true;
   }
 }

void EnableSignals()
 {
  struct sigaction sa;

  /* Configure the sigaction structure */
  //sa.sa_handler = &SignalHandler;
  sa.sa_sigaction = AdvancedSignalHandler;   /* Assign three-parameter handler */
  sigemptyset(&sa.sa_mask);                  /* Block no other signals during execution */
  sa.sa_flags = SA_SIGINFO;                  /* CRITICAL: Enables extra parameters */
  sigemptyset(&sa.sa_mask);

  /* Bind DB_UPADATE_SIGNAL to our handler function */
  sigaction(DB_UPADATE_SIGNAL, &sa, nullptr);
  /* Bind Ctrl-C signal to our handler function */
  sigaction(SIGINT, &sa, nullptr);
  /* Bind Ctrl-\ signal to our handler function */
  sigaction(SIGQUIT, &sa, nullptr);
  
  std::cout << "The signals were created successfully.\n\r";
 }



// Can be generated in the sites:
// https://acte.ltd/utils/randomkeygen  
// https://www.strongdm.com/tools/api-key-generator  
// https://emvlab.org/keyshares/



