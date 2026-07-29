#include "main.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "CommonData.h"
#include "Processes.h"

//#include <errno.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <signal.h>
#include <semaphore.h>

#include "Network.h"
#include "I2C.h"

#include "Configuration.h"

bool FullExit = false;


void CatchChildZombie();
void WaitUntilFinised(pid_t net_pid, pid_t i2c_pid);
void EnableSignals();


int main(int const argc, char const *argv[])
 {
  pid_t network_pid;
  pid_t i2c_pid;
  pid_t own_pid;

  //ansi clear screen
  printf("\033[2J\033[H");

  //code
  MasterShMem_s TskContShms;
  UNUSED(argc);
  
  own_pid = getpid();
  printf("Starting Main Car Manager Process with PID: %d\n\r", own_pid);

  InitConf(argv[0]);

  ActivateMasterShMem(&TskContShms, sizeof(TskContShmData_s));

  EnableSignals();

  network_pid = OpenProcess(NetworkProc, "Network", TskContShms.sh_mem_key, TskContShms.sem_name);
  i2c_pid = OpenProcess(I2CProc, "I2C", TskContShms.sh_mem_key, TskContShms.sem_name);

  do
   {
    CatchChildZombie();
    sleep(1);
   }
  while(!FullExit);

  sem_wait(TskContShms.p_shs);

  for(int i = 0; i < PROC_NUM_PROC_TYPES_E; i++)
   set_flag((TskContShmData_s*)TskContShms.p_shm, (ProcTypeID_e)i, true);
  sem_post(TskContShms.p_shs);

  WaitUntilFinised(network_pid, i2c_pid);


  DeactivateMasterShMem(&TskContShms);

  printf("Exitting...\n\r");
  return 0;
 }




/* Catching Zombie-Child-Processes and removing them. */ 
void CatchChildZombie()
 {
  //bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */

  int wstatus, w = 0;
  do
  {
   w = waitpid(-1, &wstatus, WNOHANG);  // WUNTRACED | WCONTINUED // WNOHANG
   if(w > 0)
    {
     printf("The process with PID: %s%d%s finished running.\n\r", (StdOutNoPiping ? PROC_PID_COLOR : ""), w, (StdOutNoPiping ? TermColorsReset : ""));
    }
  } while (w > 0);
 }

void WaitUntilFinised(pid_t net_pid, pid_t i2c_pid)
 {
  int w;
  int wstatus;

  printf("Waiting for processes to be stopped...\n\r");

  w = waitpid(i2c_pid, &wstatus, WUNTRACED );  // WUNTRACED | WCONTINUED
  if(w < 0)
   {
    perror("Was problem in wainting for the I2C process.");
   }
   
  w = waitpid(net_pid, &wstatus, WUNTRACED );  // WUNTRACED | WCONTINUED
  if(w < 0)
   {
    perror("Was problem in wainting for the Network process.");
   }
  
 }



/* Custom callback executed when signal arrives */
void AdvancedSignalHandler(int sig, siginfo_t *info, void *context) 
 {
  UNUSED(context);
  UNUSED(info);
  // if (sig == SIGUSR1)  /* Database update signal */
  //  {
  //   printf("\nSIGUSR1 signal was received successfully. \n");
  //   int passed_val = info->si_value.sival_int;  /* Reading value sent with signal from the sending program. */
  //   int process_pid = info->si_pid;             /* Reading value sent with signal from the sending program. */
  //   printf("The passed value is: %d  From process id: %d\n\r", passed_val, process_pid);
  //  }

  if (sig == SIGINT)   /* Ctrl-C Signal */
   {
    printf("\nSIGINT signal was received successfully. \n");
    FullExit = true;
   }
  
  if (sig == SIGQUIT)  /* Ctrl-\ Signal */
   {
    printf("\nSIGQUIT signal was received successfully. \n");
    FullExit = true;
   }

  if (sig == SIGPIPE)
   {
    printf("\nSIGPIPE signal was received successfully. \n");
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

  // /* Bind SIGUSR1 to our handler function */
  // sigaction(SIGUSR1, &sa, NULL);
  /* Bind Ctrl-C signal to our handler function */
  sigaction(SIGINT, &sa, NULL);
  /* Bind Ctrl-\ signal to our handler function */
  sigaction(SIGQUIT, &sa, NULL);

  sigaction(SIGPIPE, &sa, NULL);
  
  printf("The signals were created successfully.\n\r");
 }



