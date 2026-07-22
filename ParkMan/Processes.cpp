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
  // p_shm = shmat(sh_mem_id, NULL, 0);
 }

ShSemMem_c::~ShSemMem_c()
 {
  // if(p_shm != NULL)
  //  shmdt(p_shm);  // Detach
  // if(created)
  //  {
  //   shmctl(sh_mem_id, IPC_RMID, NULL); /* Shared memory control */
  //   sem_unlink(sem_name.c_str());
  //  }
  RemoveShm();
  RemoveShs();

 }


key_t ShSemMem_c::ShMemKey()
 {
  return sh_mem_key;
 }

std::string ShSemMem_c::SemName()
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
      p_shm = shmat(sh_mem_id, NULL, 0);
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
  if(p_shm != NULL)
   {
    shmdt(p_shm);  // Detach
    p_shm = NULL;
   }
  if(created)
   {
    shmctl(sh_mem_id, IPC_RMID, NULL); /* Shared memory control */
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


TaskControl_ShSM_c::TaskControl_ShSM_c()
 :ShSemMem_c(sizeof(TskContShmData_s))
 {
  ((TskContShmData_s*)p_shm)->exit_proc_flags = 0;
 }

TaskControl_ShSM_c::TaskControl_ShSM_c(key_t sh_mem_key, const char sem_name[])
 :ShSemMem_c(sh_mem_key, sem_name, sizeof(TskContShmData_s))
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
       
std::string TaskControl_ShSM_c::GetDBFileName()
 {
  return ((TskContShmData_s*)p_shm)->ControlDBPriceShMem.DBFileName;
 }

void TaskControl_ShSM_c::SetSHPFileName(std::string NameToSet)
 {
  ((TskContShmData_s*)p_shm)->ControlDBPriceShMem.SHPFileName = NameToSet;
 }
       
std::string TaskControl_ShSM_c::GetSHPFileName()
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
      perr()<<"sem_timedwait failed";
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
      ProcToOpen(Procparams.sh_mem_key, Procparams.sem_name, Procparams.ProcType);
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

Process_c::Process_c(char ProcName[], key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType)
    : TaskControl_ShSM_c(sh_mem_key, sem_name), proc_type(ProcType), proc_name(ProcName), exit_required(false), error_in_creation(false)//, p_shs(NULL), p_shm(NULL), proc_name(ProcName), sh_mem_id(-1), exit_required(false), error_in_creation(false)
 {
  //proc_name = ProcName;
  std::cout << "Entering to " << proc_name << " process...\n\rThe given sh_mem_key is: " << sh_mem_key << " and sem_name: " << sem_name << "\n\r";
  // sh_mem_id = shmget(sh_mem_key, TSK_CONT_SH_MEM_SIZE, 0666);
  // if(sh_mem_id == -1)
  //  {
  //   perr() << ProcName << " process: Error in shared memory.\n\r";
  //   error_in_creation = true;
  //   return;
  //  }
  // p_shs = sem_open(sem_name, 0, 0600);
  // if(p_shs == SEM_FAILED)
  //  {
  //   perr() << proc_name << " process: Error in shared memory semaphore.\n\r";
  //   error_in_creation = true;
  //   return;
  //  }
  // p_shm = (TskContShmData_s *)shmat(sh_mem_id, NULL, 0);
 }

Process_c::~Process_c()
 {
  // if(p_shm != NULL)
  //  shmdt(p_shm);
  std::cout << "Exitting from " << proc_name << " process... \n\r";
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
  p_shm = shmat(sh_mem_id, NULL, 0);
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

