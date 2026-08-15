#include "Processes.h"

#include <unistd.h>
#include <stdarg.h> /* Required header for variadic processing. */
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <pthread.h>

#include "Configuration.h"

#define LOG_QUEUE_NAME     "/car_pr_lg_q"   /* Attention !!!  The length mustn't exceed the strlen("NAME_LEN") - 12 definition size because in some stractures this name is stored in limited-length-char-array and to the end of this name is added a 10-digit number. */ //"/parkprice" //"/park_price"  //"/park_price_database_queue"


void *LogThread(void *Args);
void CheckLogMessageExistance(LogData_s *LogData);

/*======================================================================================================================*/


void set_flag(TskContShmData_s *TskContShmData, ProcTypeID_e flagno, bool state)
 {
  TskContShmData->exit_proc_flags = ((TskContShmData->exit_proc_flags & (~(0x01<<flagno))) | (state<<flagno));
 }

bool get_flag(TskContShmData_s *TskContShmData, ProcTypeID_e flagno)
 {
  return ((TskContShmData->exit_proc_flags >> flagno) & 0x01);
 }



/*======================================================================================================================*/



pid_t OpenProcess(subprocess_t ProcToOpen, char ProcName[], key_t sh_mem_key, char sem_name[], char sh_que_name[], char qsem_name[])
 {
  bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */

  pid_t proc_pid;
  proc_pid = fork();
  switch(proc_pid)
   {
    case -1: /* Error */
      fprintf(stderr, "%s", (StdErrNoPiping ? ResultColors[E_FAIL] : ""));
      perror("fork error.");
      fprintf(stderr, "%s", (StdErrNoPiping ? TermColorsReset : ""));
      exit(EXIT_FAILURE);
     break;
    case 0:  /* Child*/
      proc_pid = getpid();
      printf("Starting new process: %s%s%s  PID: %s%d%s\n\r", (StdOutNoPiping ? PROC_NAME_COLOR : ""),ProcName, (StdOutNoPiping ? TermColorsReset : ""), (StdOutNoPiping ? PROC_PID_COLOR : ""), proc_pid, (StdOutNoPiping ? TermColorsReset : ""));
      {
       SlaveShMem_s TskContShms;
       SlaveShQue_s TskContShqs;
       InitProcessing(&TskContShms, &TskContShqs, sh_mem_key, sem_name, sh_que_name, qsem_name);
      //  TaskSMBriefParams_s TaskSMBriefParams = {.p_shm = TskContShms.p_shm, .p_shs = TskContShms.p_shs};
      //  LogSQBriefParams_s LogSQBriefParams = {.mq = TskContShqs.mq, .p_shs = TskContShqs.p_shs };
       ProcParams_s ProcParams = {.ProcName = ProcName, .TskContShms = {.p_shm = TskContShms.p_shm, .p_shs = TskContShms.p_shs}, .TskContShqs = {.mq = TskContShqs.mq, .p_shs = TskContShqs.p_shs }};
       LogEvent(&ProcParams.TskContShqs, MakeLogMessage(E_LOG_MESSAGE, ProcName, "Stargint Process..."));
       ProcToOpen(&ProcParams);
       LogEvent(&ProcParams.TskContShqs, MakeLogMessage(E_LOG_MESSAGE, ProcName, "Finishing Process..."));
       DeinitProcessing(&TskContShms, &TskContShqs);
      }
      printf("The process %s%s%s with PID: %s%d%s finished running.\n\r", (StdOutNoPiping ? PROC_NAME_COLOR : ""),ProcName, (StdOutNoPiping ? TermColorsReset : ""), (StdOutNoPiping ? PROC_PID_COLOR : ""),proc_pid, (StdOutNoPiping ? TermColorsReset : ""));
      exit(EXIT_SUCCESS);
     break;
    default: /* Parent */
      printf("The new %s%s%s process with PID: %s%d%s started.\n\r", (StdOutNoPiping ? PROC_NAME_COLOR : ""),ProcName, (StdOutNoPiping ? TermColorsReset : ""), (StdOutNoPiping ? PROC_PID_COLOR : ""),proc_pid, (StdOutNoPiping ? TermColorsReset : ""));
      return proc_pid;
     break;
   }
 }





/*======================================================================================================================*/


void GenShMemKeyID(key_t *sh_mem_key, int *sh_mem_id, void **p_shm, size_t size)
 {
  do
   {
    *sh_mem_key = rand();
    *sh_mem_id = shmget(*sh_mem_key, size, IPC_CREAT | IPC_EXCL | 0666);
    /* The memory can be checked by the ipcs command in linux command prompt. */
   } 
  while (*sh_mem_id < 0);
  *p_shm = shmat(*sh_mem_id, NULL, 0);
 }

void GenShSemKeyID(key_t *sh_sem_key, char sem_name[], sem_t **p_shs)
 {
  do
   {
    *sh_sem_key = rand();
    snprintf(sem_name, NAME_LEN, "sem_%d", *sh_sem_key);
    *p_shs = sem_open(sem_name, O_CREAT | O_EXCL, 0600, 0);
   } 
  while (*p_shs == SEM_FAILED);
 }

void GenShQueName(char const basic_name[], char que_name[])
 {
  char tempstrg[PATH_LEN];
  struct stat st;
  do
   {
    /* code */
    sprintf(que_name, "%s_%d", basic_name, rand());
    if(que_name[0] != '/')
     {
      sprintf(tempstrg, "/%s", que_name);
      strcpy(que_name, tempstrg);
     }
    sprintf(tempstrg, "/dev/mqueue%s", que_name);
   } 
  while (stat(tempstrg, &st) == 0);
 }



void ActivateMasterShMem(MasterShMem_s *MasterShMem, int size)
 {
  GenShMemKeyID(&MasterShMem->sh_mem_key, &MasterShMem->sh_mem_id, &MasterShMem->p_shm, size);
  GenShSemKeyID(&MasterShMem->sh_sem_key,  MasterShMem->sem_name,  &MasterShMem->p_shs);
  sem_post(MasterShMem->p_shs);
 }

void DeactivateMasterShMem(MasterShMem_s *MasterShMem)
 {
  shmdt(MasterShMem->p_shm);  // Detach
  shmctl(MasterShMem->sh_mem_id, IPC_RMID, NULL); /* Shared memory control */
  sem_unlink(MasterShMem->sem_name);
 }

void ActivateSlaveShMem(SlaveShMem_s *SlaveShMem, key_t sh_mem_key, const char sem_name[], int size)
 {
  
  SlaveShMem->sh_mem_id = shmget(sh_mem_key, size, 0666);
  if(SlaveShMem->sh_mem_id == -1)
   {
    perror(" process: Error in shared memory.\n\r");
    // error_in_creation = true;
    return;
   }

  SlaveShMem->p_shs = sem_open(sem_name, 0, 0600);
  if(SlaveShMem->p_shs == SEM_FAILED)
   {
    perror(" process: Error in shared memory semaphore.\n\r");
    // error_in_creation = true;
    return;
   }
  SlaveShMem->p_shm = shmat(SlaveShMem->sh_mem_id, NULL, 0);
 }

void DeactivateSlaveShMem(SlaveShMem_s *SlaveShMem)
 {
  if(SlaveShMem->p_shm != NULL)
   shmdt(SlaveShMem->p_shm);
 }


void ActivateMasterShQue(MasterShQue_s *MasterShQue, QueueDirection_e const SendReceive, char const basic_name[], long int msg_size)
 {
  GenShQueName(basic_name, MasterShQue->sq_name);
  InitQueue(&MasterShQue->mq, SendReceive, MasterShQue->sq_name, msg_size);
  GenShSemKeyID(&MasterShQue->sh_sem_key,  MasterShQue->sem_name,  &MasterShQue->p_shs);
  sem_post(MasterShQue->p_shs);
 }

void DeactivateMasterShQue(MasterShQue_s *MasterShQue)
 {
  CloseQueue(&MasterShQue->mq, MasterShQue->sq_name);
  sem_unlink(MasterShQue->sem_name);
 }


void ActivateSlaveShQue(SlaveShQue_s *SlaveShQue, char sh_que_name[], char qsem_name[], QueueDirection_e const SendReceive, long int const msg_size)
 {
  strncpy(SlaveShQue->sq_name, sh_que_name, sizeof(SlaveShQue->sq_name));
  strncpy(SlaveShQue->sem_name, qsem_name, sizeof(SlaveShQue->sem_name));
  InitQueue(&SlaveShQue->mq, SendReceive, SlaveShQue->sq_name, msg_size);
  SlaveShQue->p_shs = sem_open(SlaveShQue->sem_name, 0, 0600);
  if(SlaveShQue->p_shs == SEM_FAILED)
   {
    perror(" process: Error in shared memory semaphore.\n\r");
    // error_in_creation = true;
    return;
   }
 }

void DeactivateSlaveShQue(SlaveShQue_s *SlaveShQue)
 {
  CloseQueue(&SlaveShQue->mq, SlaveShQue->sq_name);
 }




void InitQueue(mqd_t *mq, QueueDirection_e const SendReceive, char const que_name[], long int const msg_size)
 {
  struct mq_attr attr;

  /* Define queue attributes */
  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;        // Maximum messages in queue
  attr.mq_msgsize = msg_size; // Maximum size of any message
  attr.mq_curmsgs = 0;
  /* Create and open the queue for writing */
  switch(SendReceive)
   {
    case QUEUE_SEND_E:
      *mq = mq_open(que_name, O_CREAT | O_WRONLY, 0644, &attr);
     break;
    case QUEUE_RECEIVE_E:
      *mq = mq_open(que_name, O_CREAT | O_RDONLY, 0644, &attr);
     break;
    case QUEUE_SEND_RECEIVE_E:
      *mq = mq_open(que_name, O_CREAT | O_RDWR  , 0644, &attr);
     break;

   }
   
  if (*mq == (mqd_t)(-1)) 
   {
    perror("mq_open failed");
    exit(1);
   }

 }

void CloseQueue(mqd_t *mq, char const que_name[])
 {
  mq_close(*mq);
  mq_unlink(que_name); /* Removes queue from system completely */
 }


void InitManaging(MasterShMem_s *TskContShms, MasterShQue_s *TskContShqs, LogData_s *LogData)
 {
  ActivateMasterShMem(TskContShms, sizeof(TskContShmData_s));
  ActivateMasterShQue(TskContShqs, QUEUE_SEND_RECEIVE_E, LOG_QUEUE_NAME, sizeof(LogMessType_s));

  memset((void*)&LogData->LogParams, 0, sizeof(LogData->LogParams));
  LogData->p_sq = &TskContShqs->mq;
  LogData->Exit = false;
  pthread_create(&LogData->LogTHread, NULL, LogThread, LogData);
 }

void DeinitManaging(MasterShMem_s *TskContShms, MasterShQue_s *TskContShqs, LogData_s *LogData)
 {
  LogData->Exit = true;
  pthread_join(LogData->LogTHread, NULL);

  DeactivateMasterShQue(TskContShqs);
  DeactivateMasterShMem(TskContShms);
 }

void *LogThread(void *Args)
 {
  if(Args != NULL)
   {
    LogData_s *LogData = Args;
    InitLog(&LogData->LogParams, GetLogFilePathName());
    while(!LogData->Exit)
     {
      CheckLogMessageExistance(LogData);
     }
   }
  return NULL;
 }

void InitProcessing(SlaveShMem_s *TskContShms, SlaveShQue_s *TskContShqs, key_t sh_mem_key, const char sem_name[], char sh_que_name[], char qsem_name[])
 {
  ActivateSlaveShMem(TskContShms, sh_mem_key, sem_name, sizeof(TskContShmData_s));
  ActivateSlaveShQue(TskContShqs, sh_que_name, qsem_name, QUEUE_SEND_E, sizeof(LogMessType_s));
 }

void DeinitProcessing(SlaveShMem_s *TskContShms, SlaveShQue_s *TskContShqs)
 {
  DeactivateSlaveShMem(TskContShms);
  DeactivateSlaveShQue(TskContShqs);
 }


void CheckLogMessageExistance(LogData_s *LogData)
 {
  // bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  // bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  unsigned int prio;
  LogMessType_s ClientQueueMsg;
  struct timespec ts;

  if (clock_gettime(CLOCK_REALTIME, &ts) != -1) 
   {
    ++ts.tv_sec;
   }
  ssize_t bytes_read = mq_timedreceive(*LogData->p_sq, (char*)&ClientQueueMsg, sizeof(LogMessType_s), &prio, &ts);
  //printf("Num queue received bytes %ld\n\r", bytes_read);
  if(bytes_read > 0)
   {
    //printf("Num queue received bytes %ld\n\r", bytes_read);
    if( bytes_read >= (ssize_t)sizeof(ClientQueueMsg) )
     {
      OpenLog(&LogData->LogParams);  /* The procedure checks inside if the file is open or not. */
      AddToLog(&LogData->LogParams, ClientQueueMsg);
     }
    else
     if(bytes_read <= 0)
      CloseLog(&LogData->LogParams);
   }  
 }



void LogEvent(LogSQBriefParams_s *LogQueueParams, LogMessType_s MessageToLog)
 {
  bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  //bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */

  size_t Len;
  char buf[50];
  Len = sizeof(LogMessType_s);
  sem_wait(LogQueueParams->p_shs);
  if (mq_send(LogQueueParams->mq, (char*)&MessageToLog, Len, 0) == -1)
   {
    snprintf(buf, sizeof(buf), "%smq_send failed%s", (StdErrNoPiping ? ResultColors[E_FAIL] : ""), (StdErrNoPiping ? TermColorsReset : ""));
    perror(buf);
   } 
  else 
   {
    //std::cout << "Message sent successfully. " << Len << " bytes sent." << "\n\r";
   }
  sem_post(LogQueueParams->p_shs);

 }
