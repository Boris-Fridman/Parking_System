#include "Processes.hpp"


#include <iostream>
#include <unistd.h>
#include "Errors.hpp"
#include <stdarg.h> /* Required header for variadic processing. */

#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <fcntl.h>

#include "Configuration.hpp"
#include "Logging.hpp"


#define LOG_QUEUE_NAME     "/park_pr_lg_q"   /* Attention !!!  The length mustn't exceed the strlen("NAME_LEN") - 12 definition size because in some stractures this name is stored in limited-length-char-array and to the end of this name is added a 10-digit number. */ //"/parkprice" //"/park_price"  //"/park_price_database_queue"

/*======================================================================================================================*/


ShSemMem_c::ShSemMem_c(size_t size)
 :created(true), ShMnc(false), ShSnc(false)
 {
  LoadShm(size);
  LoadShs();

  // GenShSemKeyID(sh_sem_key, sem_name,  p_shs);
  // sem_post(p_shs);
  // GenShMemKeyID(sh_mem_key, sh_mem_id, p_shm, size);
 }

ShSemMem_c::ShSemMem_c(key_t sh_mem_key, const char sem_name[], size_t size)
 :created(false), ShMnc(false), ShSnc(false)
 {
  this->sh_mem_key = sh_mem_key;
  this->sem_name = sem_name;
  LoadShm(size);
  LoadShs();

  // p_shs = sem_open(sem_name, 0, 0600);
  // if(p_shs == SEM_FAILED)
  //  {
  //   ShSnc = true; 
  //   return;
  //  }
  // sh_mem_id = shmget(sh_mem_key, size, 0666);
  // if(sh_mem_id == -1)
  //  {
  //   ShMnc = true;
  //   return;
  //  }
  // p_shm = shmat(sh_mem_id, nullptr, 0);
 }

ShSemMem_c::~ShSemMem_c()
 {
  // if(p_shm != nullptr)
  //  shmdt(p_shm);  // Detach
  // if(created)
  //  {
  //   shmctl(sh_mem_id, IPC_RMID, nullptr); /* Shared memory control */
  //   sem_unlink(sem_name.c_str());
  //  }
  RemoveShm();
  RemoveShs();

 }


key_t ShSemMem_c::ShMemKey()
 {
  return sh_mem_key;
 }

std::string &ShSemMem_c::SemName()
 {
  return sem_name;
 }

void ShSemMem_c::LoadShm(size_t size)
 {
  if(size)
   {
    if(created)  /* "Master" Side */
     GenShMemKeyID(sh_mem_key, sh_mem_id, p_shm, size);
    else         /* "Slave" Side  */
     {
      sh_mem_id = shmget(sh_mem_key, size, 0666);
      if(sh_mem_id == -1)
       {
        ShMnc = true;  /* Shered memory not created    */
        return;
       }
      p_shm = shmat(sh_mem_id, nullptr, 0);
     }
   }
 }

void ShSemMem_c::LoadShs()
 {
  if(created)  /* "Master" Side */
   {
    GenShSemKeyID(sh_sem_key, sem_name,  p_shs);
    sem_post(p_shs);
   }
  else         /* "Slave" Side  */
   {
    p_shs = sem_open(sem_name.c_str(), 0, 0600);
    if(p_shs == SEM_FAILED)
     {
      ShSnc = true; /* Shared Semaphore not created */
      return;
     }
   }
 }

void ShSemMem_c::RemoveShm()
 {
  if(p_shm != nullptr)
   {
    shmdt(p_shm);  // Detach
    p_shm = nullptr;
   }
  if(created)
   {
    shmctl(sh_mem_id, IPC_RMID, nullptr); /* Shared memory control */
   }
 }

void ShSemMem_c::RemoveShs()
 {
  if(created)
   {
    sem_unlink(sem_name.c_str());
   }
 }










/*======================================================================================================================*/



ShSemMemQue_c::ShSemMemQue_c(size_t shmem_size, QueueDirection_e queue_direction, std::string queu_basic_name, size_t shque_size)
 :ShSemMem_c(shmem_size)
 {
  LoadShq(queue_direction, queu_basic_name, shque_size);
  LoadShqs();
 }


ShSemMemQue_c::ShSemMemQue_c(key_t sh_mem_key, const char sem_name[], std::string sq_name, std::string qsem_name, size_t shmem_size, QueueDirection_e queue_direction, size_t shque_size)
 :ShSemMem_c(sh_mem_key, sem_name, shmem_size)
 {
  this->sq_name = sq_name;
  this->qsem_name = qsem_name;
  LoadShq(queue_direction, "", shque_size);
  LoadShqs();
 }


ShSemMemQue_c::~ShSemMemQue_c()
 {
  RemoveShq();
  RemoveShqs();
 }





void ShSemMemQue_c::LoadShq(QueueDirection_e SendReceive, std::string const basic_name, size_t size)
 {
  bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */

  struct mq_attr attr;

  /* Define queue attributes */
  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;     // Maximum messages in queue
  attr.mq_msgsize = size;  // Maximum size of any message
  attr.mq_curmsgs = 0;

  if(created)  /* "Master" Side */
   {
    GenShQueName(basic_name, sq_name);
   }
  else         /* "Slave" Side  */
   {
   }
  switch(SendReceive)
   {
    case QUEUE_SEND_E:
      p_sq = mq_open(sq_name.c_str(), O_CREAT | O_WRONLY, 0644, &attr);
     break;
    case QUEUE_RECEIVE_E:
      p_sq = mq_open(sq_name.c_str(), O_CREAT | O_RDONLY, 0644, &attr);
     break;
   }
  if (p_sq == (mqd_t)(-1)) 
   {
    perr() << (StdErrNoPiping ? TermRed : "") << "mq_open failed" << (StdErrNoPiping ? TermColorsReset : "");
    //exit(1);
   }
  else
   {
    std::cout << (StdOutNoPiping ? TermBrightBlue : "") << "The queue was opened successfully: "<< (StdOutNoPiping ? TermBrightCyan : "") << p_sq << " " << sq_name << (StdOutNoPiping ? TermColorsReset : "") << "\n\r";
   }
 }


void ShSemMemQue_c::LoadShqs()
 {
  bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */

  if(created)  /* "Master" Side */
   {
    GenShSemKeyID(sh_qsem_key, qsem_name,  p_shqs);
    std::cout << (StdOutNoPiping ? TermBrightBlue : "") << "The semaphore was generated and created successfully: "<< (StdOutNoPiping ? TermBrightCyan : "") << p_shqs << " " << qsem_name << (StdOutNoPiping ? TermColorsReset : "") << "\n\r";
   }
  else         /* "Slave" Side  */
   {
    p_shqs = sem_open(qsem_name.c_str(), 0, 0600);
    if(p_shqs == SEM_FAILED)
     {
      perr() << (StdErrNoPiping ? TermRed : "") << "The semaphore wasn't created" << (StdErrNoPiping ? TermColorsReset : "");
      //ShSnc = true; /* Shared Semaphore not created */
      return;
     }
    std::cout << (StdOutNoPiping ? TermBrightBlue : "") << "The semaphore was opened successfully: "<< (StdOutNoPiping ? TermBrightCyan : "") << p_shqs << " " << qsem_name << (StdOutNoPiping ? TermColorsReset : "") << "\n\r";
   }
  sem_post(p_shqs);
 }


void ShSemMemQue_c::RemoveShq()
 {
  if(created)  /* "Master" Side */
   {
   }
  else         /* "Slave" Side  */
   {
   }
  mq_close(p_sq);
  mq_unlink(sq_name.c_str()); /* Removes queue from system completely */
 }


void ShSemMemQue_c::RemoveShqs()
 {
  if(created)  /* "Master" Side */
   {
    sem_unlink(qsem_name.c_str());
   }
  else         /* "Slave" Side  */
   {
   }
 }


std::string &ShSemMemQue_c::QueueName()
 {
  return sq_name;
 }

std::string &ShSemMemQue_c::QSemName()
 {
  return qsem_name;
 }












/*======================================================================================================================*/

TaskControl_ShSM_c::TaskControl_ShSM_c()
 :ShSemMemQue_c(sizeof(TskContShmData_s), QUEUE_RECEIVE_E, LOG_QUEUE_NAME, sizeof(LogMessType_s))
 {
  ((TskContShmData_s*)p_shm)->exit_proc_flags = 0;
 }

TaskControl_ShSM_c::TaskControl_ShSM_c(key_t sh_mem_key, const char sem_name[], std::string sq_name, std::string qsem_name)
 :ShSemMemQue_c(sh_mem_key, sem_name, sq_name, qsem_name, sizeof(TskContShmData_s), QUEUE_SEND_E,  sizeof(LogMessType_s))
 {
  ((TskContShmData_s*)p_shm)->exit_proc_flags = 0;
 }


void TaskControl_ShSM_c::ExitProcess(ProcTypeID_e ProcToExit)
 {
  sem_wait(p_shs);
  ((TskContShmData_s*)p_shm)->set_flag(ProcToExit, true);
  sem_post(p_shs);
 }

void TaskControl_ShSM_c::ExitAllProcesses()
 {
  int i;
  sem_wait(p_shs);
  for(i = 0; i < PROC_NUM_PROC_TYPES_E; i++)
   ((TskContShmData_s*)p_shm)->set_flag((ProcTypeID_e)i, true);
  sem_post(p_shs);
 } 

bool TaskControl_ShSM_c::ProcessMustExit(ProcTypeID_e ProcToExit)
 {
  return ((TskContShmData_s*)p_shm)->get_flag(ProcToExit);
 }

void TaskControl_ShSM_c::SetDBFileName(std::string NameToSet)
 {
  ((TskContShmData_s*)p_shm)->ControlDBPriceShMem.DBFileName = NameToSet;
 }
       
std::string &TaskControl_ShSM_c::GetDBFileName()
 {
  return ((TskContShmData_s*)p_shm)->ControlDBPriceShMem.DBFileName;
 }

void TaskControl_ShSM_c::SetSHPFileName(std::string NameToSet)
 {
  ((TskContShmData_s*)p_shm)->ControlDBPriceShMem.SHPFileName = NameToSet;
 }
       
std::string &TaskControl_ShSM_c::GetSHPFileName()
 {
  return ((TskContShmData_s*)p_shm)->ControlDBPriceShMem.SHPFileName;
 }

void TaskControl_ShSM_c::ReloadDatabase()
 {
  sem_wait(p_shs);
  ((TskContShmData_s*)p_shm)->ControlDBPriceShMem.DBUpdateRequired = true;
  sem_post(p_shs);
 }

bool TaskControl_ShSM_c::DataBaseMustBeReloaded()
 {
  bool Result;

  struct timespec ts;
  if (clock_gettime(CLOCK_REALTIME, &ts) != -1) 
   {
    ++ts.tv_sec;
   }
  if(sem_timedwait(p_shs, &ts) == -1)
   {
    if (errno == ETIMEDOUT) 
     {
      std::cout << "Timeout reached! Semaphore was not available.\n";
     } 
    else if (errno == EINTR) 
     {
      std::cout << "The call was interrupted by a signal handler.\n";
     } 
    else 
     {
      perr() << "sem_timedwait failed";
     }
    return false;
   }
  else
   {
    //sem_wait(p_shs);
    Result = ((TskContShmData_s*)p_shm)->ControlDBPriceShMem.DBUpdateRequired;
    ((TskContShmData_s*)p_shm)->ControlDBPriceShMem.DBUpdateRequired = false;
    sem_post(p_shs);
    return Result;

   }
  return false;
  // sem_wait(p_shs);
  // Result = ((TskContShmData_s*)p_shm)->ControlDBPriceShMem.DBUpdateRequired;
  // ((TskContShmData_s*)p_shm)->ControlDBPriceShMem.DBUpdateRequired = false;
  // sem_post(p_shs);
  // return Result;
 }





/*======================================================================================================================*/


pid_t OpenProcess(subprocess_t ProcToOpen, ProcParams_s Procparams, char ProcName[])
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
    case 0:  /* Child */
      proc_pid = getpid();
      printf("Starting new process: %s%s%s  PID: %s%d%s\n\r", (StdOutNoPiping ? PROC_NAME_COLOR : ""),ProcName, (StdOutNoPiping ? TermColorsReset : ""), (StdOutNoPiping ? PROC_PID_COLOR : ""), proc_pid, (StdOutNoPiping ? TermColorsReset : ""));
      ProcToOpen(Procparams.sh_mem_key, Procparams.sem_name, Procparams.sq_name, Procparams.qsem_name, Procparams.ProcType);
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

void *Thread(void *attr)
 {
  UNUSED(attr);
  return nullptr;
 }
void OpenThread()
 {
  pthread_t ThreadID;
  TaskControl_ShSM_c ThreadParams;
  //pthread_attr_t Attr;
  pthread_create(&ThreadID, nullptr, Thread, (void*)&ThreadParams);
 }




/*======================================================================================================================*/

Process_c::Process_c(char ProcName[], key_t sh_mem_key, const char sem_name[], std::string sq_name, std::string qsem_name, ProcTypeID_e ProcType)
    : TaskControl_ShSM_c(sh_mem_key, sem_name, sq_name, qsem_name), proc_type(ProcType), proc_name(ProcName), exit_required(false), error_in_creation(false)
 {
  LogMessType_s MessageToLog;

  std::cout << "Entering to " << proc_name << " process...\n\rThe given sh_mem_key is: " << sh_mem_key << " and sem_name: " << sem_name << "\n\r";

  MessageToLog = MakeLogMessage(LOG_MESSAGE, proc_type, proc_name.c_str(), "The process started running.");
  LogEvent(MessageToLog);
 }

Process_c::~Process_c()
 {
  LogMessType_s MessageToLog;

  std::cout << "Exitting from " << proc_name << " process... \n\r";

  MessageToLog = MakeLogMessage(LOG_MESSAGE, proc_type, proc_name.c_str(), "The process Finished running.");
  LogEvent(MessageToLog);
 }


/* Breaks the default loop existing in the RunProcess. */
void Process_c::MakeExit()
 { 
  exit_required = true;
 }



/* This procedure contains main loop with exit condition where is running the "OnRunProcess()" procedure, but can be overwritten according to requirements. */
void Process_c::RunProcess()
 {
  OnStartProcess();
  while (!(exit_required || error_in_creation))
   {
    OnRunProcess();
    CheckExitStatus();
   }  
  OnFinishProcess();
 }

/* This procedure is empty and runs before starting running the process for any initializations. */ 
void Process_c::OnStartProcess()     
 {

 }



/* This procedure contains the 1 second sleep and runs in the loop of the "RunProcess()" procedure, but can be overwritten. */
void Process_c::OnRunProcess()
 {
  sleep(1);
 }

/* This procedure is empty and runs after finishing running the process for any deinitializations. */ 
void Process_c::OnFinishProcess()    
 {

 }

/* This procedure contains the exit checking conditions and runs in the loop of the "RunProcess()" procedure, but can be overwritten. */ 
void Process_c::CheckExitStatus()
 {
  exit_required |= ProcessMustExit(proc_type);  //((TskContShmData_s *)p_shm)->get_flag(proc_type);  //( p_shm->exit_proc_flags & (0x1 << proc_type) );     
  exit_required |= (getppid() == 1);            // Checking if the parent process is running. If not enables exit.
 }   


void Process_c::LogEvent(LogMessType_s MessageToLog)
 {
  size_t Len;
  Len = sizeof(LogMessType_s);
  sem_wait(p_shqs);
  if (mq_send(p_sq, (char*)&MessageToLog, Len, 0) == -1)
   {
    perror("mq_send failed");
   } 
  else 
   {
    printf("Message sent successfully. %ld bytes sent.\n\r", Len);
   }
  sem_post(p_shqs);
 }



/*======================================================================================================================*/


void GenShMemKeyID(key_t &sh_mem_key, int &sh_mem_id, void *&p_shm, size_t size)
 {
  do
   {
    sh_mem_key = rand();
    sh_mem_id = shmget(sh_mem_key, size, IPC_CREAT | IPC_EXCL | 0666);
    /* The memory can be checked by the ipcs command in linux command prompt. */
   } 
  while (sh_mem_id < 0);
  p_shm = shmat(sh_mem_id, nullptr, 0);
 }

void GenShSemKeyID(key_t &sh_sem_key, std::string &sem_name, sem_t *&p_shs)
 {
  do
   {
    sh_sem_key = rand();
   
    std::ostringstream stream;
    stream << "sem_" << sh_sem_key;
    sem_name = stream.str();
    
    p_shs = sem_open(sem_name.c_str(), O_CREAT | O_EXCL, 0600, 0);
   } 
  while (p_shs == SEM_FAILED);
 }

void GenShQueName(std::string const &basic_name, std::string &que_name)
 {
  std::string RandSoufix;
  struct stat st;
  do
   {
    /* code */
    RandSoufix = std::to_string(rand());
    que_name = basic_name + "_" + RandSoufix;
    //que_name = basic_name;
    if(que_name.c_str()[0] != '/')
     que_name = "/" + que_name;
   } 
  while (stat(("/dev/mqueue" + que_name).c_str(), &st) == 0);
 }



/*======================================================================================================================*/

ProcMan_c::ProcMan_c()
  :TaskControl_ShSM_c(), LogParams({false, nullptr, {0}, '\t'})
 {
  memset((void*)&LogParams, 0, sizeof(LogParams));
  LoadLogThread();
 }


ProcMan_c::~ProcMan_c()
 {
  CloseLogThread();
  pthread_join(LogTHread, nullptr);
 }




void ProcMan_c::LoadLogThread()
 {
  pthread_create(&LogTHread, NULL, ProcMan_c::StatLogThread, this);
 }


void ProcMan_c::CloseLogThread()
 {
  Exit = true;
 }


void *ProcMan_c::StatLogThread(void *Args)
 {
  ProcMan_c *instance = static_cast<ProcMan_c *>(Args);
  return instance->LogThread(Args);
 }

void *ProcMan_c::LogThread(void *Args)
 {
  UNUSED(Args);
  
  InitLog(&LogParams, GetLogFilePathName());
  while(!Exit)
   {
    CheckLogMessageExistance();
   }
  
  return nullptr;
 }


void ProcMan_c::CheckLogMessageExistance()
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
  ssize_t bytes_read = mq_timedreceive(p_sq, (char*)&ClientQueueMsg, sizeof(LogMessType_s), &prio, &ts);
  //std::cout << "Num queue received bytes " << bytes_read << "\n\r";
  if(bytes_read > 0)
   {
    //std::cout << "Num queue received bytes " << bytes_read << "\n\r";
    if( bytes_read >= (ssize_t)sizeof(ClientQueueMsg) )
     {
      OpenLog(&LogParams);  /* The procedure checks inside if the file is open or not. */
      AddToLog(&LogParams, ClientQueueMsg);
     }
    else
     if(bytes_read <= 0)
      CloseLog(&LogParams);
   }  
 }
 