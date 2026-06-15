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
#include "Processes.hpp"





#define SIGNAL_TO_TERMINATE   SIGINT





void CatchChildZombie();
void WaitUntilFinised();

void CreatePIDFile(int const argc, char const *argv[], char FileName[]);
void RemovePIDFile(char FileName[]);
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

// struct ShmemPar_s
//  {
//   key_t sh_mem_key;          /* Task Control shared memroy key           */                              // Is used as reference for other side process.
//   int sh_mem_id;             /* Task Control shared memroy ID            */  // Is used for removing.
//   void *p_shm;               /* Pointer to Task Control shared memory    */  // Is used for detatching.                                                   // Is used as reference for accessing.
//  };

// struct ShsemPar_s
//  {
//   key_t sh_sem_key;          /* Task Control shared semaphore key        */
//   sem_t *p_shs;              /* Pointer to Task Control shared semaphore */                                                                               // Is used as reference for accessing.
//   std::string sem_name = ""; /* Task Control shared semaphore name       */  // Is used for unlinking.   // Is used for sharing by other side process.
//  };


bool FullExist = false;

/*----------------------------------------------------------------------------------------------------------------------*/
/*  Main function from which the program starts running.                                                                */
int main(int const argc, char const *argv[])
 {
  char PIDFileName[PATH_LEN];
  
  

  key_t tsk_cont_sh_mem_key;          /* Task Control shared memroy key           */                              // Is used as reference for other side process.
  key_t tsk_cont_sh_sem_key;          /* Task Control shared semaphore key        */
  int tsk_cont_sh_mem_id;             /* Task Control shared memroy ID            */  // Is used for removing.
  sem_t *p_tsk_cont_shs;              /* Pointer to Task Control shared semaphore */                                                                               // Is used as reference for accessing.
  TskContShmData_s *p_tsk_cont_shm;          /* Pointer to Task Control shared memory    */  // Is used for detatching.                                                   // Is used as reference for accessing.
  std::string tsk_cont_sem_name = ""; /* Task Control shared semaphore name       */  // Is used for unlinking.   // Is used for sharing by other side process.

  // key_t db_sh_mem_key;          /* Task Control shared memroy key           */                              // Is used as reference for other side process.
  // key_t db_sh_sem_key;          /* Task Control shared semaphore key        */
  // int db_sh_mem_id;             /* Task Control shared memroy ID            */  // Is used for removing.
  // sem_t *p_db_shs;              /* Pointer to Task Control shared semaphore */                                                                               // Is used as reference for accessing.
  // PriceTab_s *p_db_shm;         /* Pointer to Task Control shared memory    */  // Is used for detatching.                                                   // Is used as reference for accessing.
  // std::string db_sem_name = ""; /* Task Control shared semaphore name       */  // Is used for unlinking.   // Is used for sharing by other side process.


  GPS_Cords_s TelAviv = {32.0853, 34.7818}, Jerusalem = {31.7683, 35.2137};
  double d;
  d = GetDistance(TelAviv, Jerusalem);

  std::cout << std::fixed << std::setprecision(6);
  
  std::cout << d << "\n\r";
 
  own_pid = getpid();

  std::cout << "The current protram is: "<< argv[0] <<"\n\r";
  std::cout << "The pid is: " << own_pid << "\n\r";

  CreatePIDFile(argc, argv, PIDFileName);
  EnableSignals();

  GetShMemKeyID(tsk_cont_sh_mem_key, tsk_cont_sh_mem_id, *((void**)&p_tsk_cont_shm), TSK_CONT_SH_MEM_SIZE);
  GenShSemKeyID(tsk_cont_sh_sem_key, tsk_cont_sem_name, p_tsk_cont_shs);

  //p_tsk_cont_shm = (TskContShmData_s *)shmat(tsk_cont_sh_mem_id, NULL, 0);  // Attach 
  //p_tsk_cont_shm->exit_proc_flags = 0;
  sem_post(p_tsk_cont_shs);



  database_pid = fork();
  switch(database_pid)
   {
    case -1:
      perr() << "fork error.";
      exit(EXIT_FAILURE);
     break;
    case 0:
      std::cout << "Starting DataBase process\n\r";
      DataBaseProc(tsk_cont_sh_mem_key, tsk_cont_sem_name.c_str() , PROC_DATABASE_E);
      std::cout << "Finishing DataBase process\n\r";
      exit(EXIT_SUCCESS);
     break;
    default:
      std::cout << "The DataBase process started with PID: "<< database_pid <<"\n\r";
     break;
   }
   
  parking_pid = fork();
  switch(parking_pid)
   {
    case -1:
      perr()<<"fork error.";
      exit(EXIT_FAILURE);
     break;
    case 0:
      std::cout << "Starting Parking process\n\r";
      ParkingProc(tsk_cont_sh_mem_key, tsk_cont_sem_name.c_str(), PROC_PARKING_E);
      std::cout << "Finishing Parking process\n\r";
      exit(EXIT_SUCCESS);
     break;
    default:
      std::cout << "The Parking process started with PID: " << parking_pid << "\n\r";
     break;
   }

  network_pid = fork();
  switch(network_pid)
   {
    case -1:
      perr() << "fork error.";
      exit(EXIT_FAILURE);
     break;
    case 0:
      std::cout << "Starting Network process\n\r";
      NetworkProc(tsk_cont_sh_mem_key, tsk_cont_sem_name.c_str(), PROC_NETWORK_E);
      std::cout << "Finishing Network process\n\r";
      exit(EXIT_SUCCESS);
     break;
    default:
      std::cout << "The Network process started with PID: " << network_pid << "\n\r";
     break;
   }

//  sleep(10);
//  p_tsk_cont_shm->set_flag(PROC_DATABASE_E, true);
//  sleep(10);
//  p_tsk_cont_shm->set_flag(PROC_NETWORK_E, true);
//  sleep(10);
//  p_tsk_cont_shm->set_flag(PROC_PARKING_E, true);
//  sleep(10);
 //p_tsk_cont_shm->exit_proc_flags = 0xFF;  // For test only.

  std::cout << "The loop is infinite. So press Ctrl+C to quit.\n\r";
  do
   {
    /* code */
    CatchChildZombie();
   } while (!FullExist);



  sem_wait(p_tsk_cont_shs);
  //p_tsk_cont_shm -> exit_proc_flags = (-1);  /* The value is set to "-1" to enable all the flags. The reason why "-1" and not 0xFF is to put the value to maximal independently of the variable size. */
  for(int i = 0; i < PROC_NUM_PROC_TYPES_E; i++)
   p_tsk_cont_shm->set_flag((ProcTypeID_e)i, true);
  sem_post(p_tsk_cont_shs);
  

  WaitUntilFinised();

  RemovePIDFile(PIDFileName);  /* Removes file with the main pid of this program. */

  shmdt(p_tsk_cont_shm);  // Detach
  shmctl(tsk_cont_sh_mem_id, IPC_RMID, NULL); /* Shared memory control */
  sem_unlink(tsk_cont_sem_name.c_str());

  std::cout << "The program finished running.\n\r";
  return 0;
 }







/* Catching Zombie-Child-Processes and removing them. */ 
void CatchChildZombie()
 {
  int wstatus, w = 0;
  do
  {
   w = waitpid(-1, &wstatus, WNOHANG);  // WUNTRACED | WCONTINUED // WNOHANG
   if(w > 0)
    {
     std::cout << "The process with PID: " << w << " finished running.\n\r";
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

void CreatePIDFile(int const argc, char const *argv[], char FileName[])
 {
  int pid;
  GetPIDFile(argc, argv, FileName);
  pid = getpid();

  std::ofstream outFile(FileName);
  if(outFile.is_open())
   {
    outFile<<pid;
    outFile.close();
   }
  std::cout<<FileName<<"\n\r";

 }

void RemovePIDFile(char FileName[])
 {
  remove(FileName);
 }


// Custom callback executed when signal arrives
void AdvancedSignalHandler(int sig, siginfo_t *info, void *context) 
 {
  UNUSED(context);
  if (sig == DB_UPADATE_SIGNAL)  /* Database update signal */
   {
    std::cout << "\nDB_UPADATE_SIGNAL signal was received successfully. \n";
    int passed_val = info->si_value.sival_int;  /* Reading value sent with signal from the sending program. */
    int process_pid = info->si_pid;             /* Reading value sent with signal from the sending program. */
    std::cout << "The passed value is: " << passed_val << "  From process id: " << process_pid << "\n\r";
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
  sigaction(DB_UPADATE_SIGNAL, &sa, NULL);
  /* Bind Ctrl-C signal to our handler function */
  sigaction(SIGINT, &sa, NULL);
  /* Bind Ctrl-\ signal to our handler function */
  sigaction(SIGQUIT, &sa, NULL);
  
  

  std::cout << "The signals were created successfully.\n\r";
 }



// Can be generated in the sites:
// https://acte.ltd/utils/randomkeygen  
// https://www.strongdm.com/tools/api-key-generator  
// https://emvlab.org/keyshares/




