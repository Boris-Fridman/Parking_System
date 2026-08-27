/*======================================================================================================================*/

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

//#define CONVERT_TO_DAEMON
//#define BEAGLE_BONE

#ifdef CONVERT_TO_DAEMON  
#include <syslog.h>
#endif

/*======================================================================================================================*/

bool FullExit = false;

/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Functions / Procedures for internal usage.
 * *************************************************************************************************************
 */

void CatchChildZombie(LogSQBriefParams_s *LogSQBriefParams);
void WaitUntilFinised(pid_t net_pid, pid_t i2c_pid);
void EnableSignals();

/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Main Function from which the program starts running. 
 * *************************************************************************************************************
 */

int main(int const argc, char const *argv[])
 {
  UNUSED(argc);
  UNUSED(argv);

  // //ansi clear screen
  // printf("\033[2J\033[H");

  //code

  char OwnName[PATH_LEN];
  bool PrevProgRunning;
  GetOwnNamePath(OwnName, sizeof(OwnName));

#if defined(CONVERT_TO_DAEMON) || defined (BEAGLE_BONE)
  PrevProgRunning = PrevProcCopyRunning(OwnName, false); /* In the daemon mode the program cannot run in several copies anyway neither from the same path nor from different pathes. In case of BeagleBone version the multicopy running is impossible anyway because they can shere the same I2C Prot. */
#else
  PrevProgRunning = PrevProcCopyRunning(OwnName, true);  /* In regular application mode the program cannot run from the same path in several copies, but from different pathes copy per path. */
#endif  

  if(PrevProgRunning)
   {
    printf("The program is allready running.\n\r");
    exit(-2);
   }


#ifdef CONVERT_TO_DAEMON  
  printf("Starting CarMan Daemon...\n\r");
  openlog("carman_daemon_test", LOG_PID, LOG_DAEMON);
  if(daemon(0, 0) == -1)
   {
    syslog(LOG_ERR, "Failed to daemonize process.");
    closelog();
    return -1;
   }
  else
   {
    syslog(LOG_INFO, "The program was started as a daemon successfully.");
   }
#else
  printf("Starting CarMan Program...\n\r");
#endif

  pid_t network_pid;
  pid_t i2c_pid;
  pid_t own_pid;

  MasterShMem_s TskContShms;
  MasterShQue_s TskContShqs;
  LogData_s TskContLogData;

  own_pid = getpid();
  printf("Starting Main Car Manager Process with PID: %d\n\r", own_pid);

  InitConfiguration(OwnName);

  InitManaging(&TskContShms, &TskContShqs, &TskContLogData);

  LogSQBriefParams_s LogSQBriefParams = {.mq = TskContShqs.mq, .p_shs = TskContShqs.p_shs};

  LogEvent(&LogSQBriefParams , MakeLogMessage(E_LOG_MESSAGE, MAIN_PROC_NAME, "Starting CarMan Program..."));

  EnableSignals();
  
  network_pid = OpenProcess(NetworkProc, NETW_PROC_NAME, TskContShms.sh_mem_key, TskContShms.sem_name, TskContShqs.sq_name, TskContShqs.sem_name);
  i2c_pid = OpenProcess(I2CProc, I2C_PROC_NAME, TskContShms.sh_mem_key, TskContShms.sem_name, TskContShqs.sq_name, TskContShqs.sem_name);

  do
   {
    CatchChildZombie(&LogSQBriefParams);
    sleep(1);
   }
  while(!FullExit);

  sem_wait(TskContShms.p_shs);

  for(int i = 0; i < PROC_NUM_PROC_TYPES_E; i++)
   set_flag((TskContShmData_s*)TskContShms.p_shm, (ProcTypeID_e)i, true);
  sem_post(TskContShms.p_shs);

  WaitUntilFinised(network_pid, i2c_pid);


  LogEvent(&LogSQBriefParams , MakeLogMessage(E_LOG_MESSAGE, MAIN_PROC_NAME, "The CarMan Program finished running."));


  DeinitManaging(&TskContShms, &TskContShqs, &TskContLogData);

  printf("Exitting...\n\r");

#ifdef CONVERT_TO_DAEMON  
  syslog(LOG_INFO, "The protram finished running.");
  closelog();
#endif

  return 0;
 }


/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Process-Analizing Functions / Procedures.
 * *************************************************************************************************************
 */

/*----------------------------------------------------------------------------------------------------------------------*/
/* Catching Zombie-Child-Processes and removing them.                                                                   */ 
void CatchChildZombie(LogSQBriefParams_s *LogSQBriefParams)
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
     if(LogSQBriefParams != NULL)
      {
       char buf[100];
       snprintf(buf, sizeof(buf), "The process with PID: %d finished running emergencely.", w);
       LogEvent(LogSQBriefParams, MakeLogMessage(E_LOG_FAIL, MAIN_PROC_NAME, buf));
      }
    }
  } while (w > 0);
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Waiting processes untill they finish befor exitting the main process.                                                */
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

/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Signal processing Functions / Procedures.
 * *************************************************************************************************************
 */

/*----------------------------------------------------------------------------------------------------------------------*/
/* Custom callback executed when signal arrives.                                                                        */
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

/*----------------------------------------------------------------------------------------------------------------------*/
/* Enables the selected signals that are processed by the signal handler.                                               */
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


/*======================================================================================================================*/
