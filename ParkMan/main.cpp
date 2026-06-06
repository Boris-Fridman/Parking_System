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





#define SIGNAL_TO_TERMINATE   SIGINT




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

bool FullExist = false;

/*----------------------------------------------------------------------------------------------------------------------*/
/*  Main function from which the program starts running.                                                                */
int main(int const argc, char const *argv[])
 {
  char PIDFileName[PATH_LEN];

  // Can be generated in the sites:
  // https://acte.ltd/utils/randomkeygen  
  // https://www.strongdm.com/tools/api-key-generator  
  // https://emvlab.org/keyshares/
  key_t sh_mem_key;
  key_t sh_sem_key;
  int sh_mem_id;
  sem_t *p_shs;
  ShmData_s *p_shm;
  //char sem_name[20] = {0};
  std::string sem_name = "";

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


  do
   {
    /* code */
    sh_mem_key = rand();
    sh_mem_id = shmget(sh_mem_key, SH_MEM_SIZE, IPC_CREAT | IPC_EXCL | 0666);
    // The memory can be checked by the ipcs command in linux command prompt.
   } 
  while (sh_mem_id < 0);
  do
   {
    /* code */
    sh_sem_key = rand();
    //sh_sem_id = semget(sh_sem_key, 1, IPC_CREAT | IPC_EXCL | 0666);`
    //snprintf(sem_name, sizeof(sem_name), "sem_%d", sh_sem_key);
   
    std::ostringstream stream;
    stream << "sem_" << sh_sem_key;
    sem_name = stream.str();
    
    // if(sem_name.length() > 19) sem_name = sem_name.substr(0, 19);
    p_shs = sem_open(sem_name.c_str(), O_CREAT | O_EXCL, 0600, 0);
    //p_shs = sem_open(sem_name, O_CREAT | O_EXCL, 0600, 0);
   } 
  while (p_shs == SEM_FAILED);


  p_shm = (ShmData_s *)shmat(sh_mem_id, NULL, 0);  // Attach 
  p_shm->exit_database = false;
  p_shm->exit_network = false;
  p_shm->exit_parking = false;
  p_shm->exit_existerrors = false;
  sem_post(p_shs);



  database_pid = fork();
  switch(database_pid)
   {
    case -1:
      perr() << "fork error.";
      exit(EXIT_FAILURE);
     break;
    case 0:
      std::cout << "Starting DataBase process\n\r";
      DataBaseProc(sh_mem_key, sem_name.c_str());
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
      DataBaseProc(sh_mem_key, sem_name.c_str());
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
      NetworkProc(sh_mem_key, sem_name.c_str());
      std::cout << "Finishing Network process\n\r";
      exit(EXIT_SUCCESS);
     break;
    default:
      std::cout << "The Network process started with PID: " << network_pid << "\n\r";
     break;
   }

// sleep(10);

 std::cout << "The loop is infinite. So press Ctrl+C to quit.\n\r";
  do
  {
     /* code */
 
  } while (!FullExist);



  sem_wait(p_shs);
  p_shm->exit_database = true;
  p_shm->exit_network = true;
  p_shm->exit_parking = true;
  sem_post(p_shs);
  

  WaitUntilFinised();

  // std::cout << "The loop is infinite. So press Ctrl+C to quit.\n\r";
  // do
  // {
  //    /* code */
 
  // } while (1);

  RemovePIDFile(PIDFileName);

  shmdt(p_shm);  // Detach
  shmctl(sh_mem_id, IPC_RMID, NULL); // Shared memory control
  sem_unlink(sem_name.c_str());

  std::cout << "The program finished running.\n\r";
  return 0;
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
  std::cout<<FileName;

 }

void RemovePIDFile(char FileName[])
 {
  remove(FileName);
 }


// Custom callback executed when signal arrives
void AdvancedSignalHandler(int sig, siginfo_t *info, void *context) 
 {
  UNUSED(context);
  if (sig == DB_UPADATE_SIGNAL)  // Database update signal
   {
    std::cout << "\nDB_UPADATE_SIGNAL signal was received successfully. \n";
    int passed_val = info->si_value.sival_int;  // Reading value sent with signal from the sending program.
    int process_pid = info->si_pid;             // Reading value sent with signal from the sending program.
    std::cout << "The passed value is: " << passed_val << "  From process id: " << process_pid << "\n\r";
   }

  if (sig == SIGINT)  // Ctrl-C Signal
   {
    std::cout << "\nSIGINT signal was received successfully. \n";
    FullExist = true;
   }
  
  if (sig == SIGQUIT)  // Ctrl-\ Signal
   {
    std::cout << "\nSIGQUIT signal was received successfully. \n";
    FullExist = true;
   }
 }

void EnableSignals()
 {
  struct sigaction sa;

  // Configure the sigaction structure
  //sa.sa_handler = &SignalHandler;
  sa.sa_sigaction = AdvancedSignalHandler;   // Assign three-parameter handler
  sigemptyset(&sa.sa_mask);                  // Block no other signals during execution
  sa.sa_flags = SA_SIGINFO;                  // CRITICAL: Enables extra parameters
  sigemptyset(&sa.sa_mask);

  // Bind DB_UPADATE_SIGNAL to our handler function
  sigaction(DB_UPADATE_SIGNAL, &sa, NULL);
  // Bind Ctrl-C signal to our handler function
  sigaction(SIGINT, &sa, NULL);
  // Bind Ctrl-\ signal to our handler function
  sigaction(SIGQUIT, &sa, NULL);
  
  

  std::cout << "The signals were created successfully.\n\r";
 }



