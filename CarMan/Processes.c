#include "Processes.h"

#include <unistd.h>
#include <stdarg.h> /* Required header for variadic processing. */
#include <fcntl.h>


void set_flag(TskContShmData_s *TskContShmData, ProcTypeID_e flagno, bool state)
 {
  TskContShmData->exit_proc_flags = ((TskContShmData->exit_proc_flags & (~(0x01<<flagno))) | (state<<flagno));
 }

bool get_flag(TskContShmData_s *TskContShmData, ProcTypeID_e flagno)
 {
  return ((TskContShmData->exit_proc_flags >> flagno) & 0x01);
 }



/*======================================================================================================================*/



pid_t OpenProcess(subprocess_t ProcToOpen, char ProcName[], key_t sh_mem_key, char sem_name[])
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
      ProcToOpen(sh_mem_key, sem_name);
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
    // perror(" process: Error in shared memory.\n\r");
    // error_in_creation = true;
    return;
   }

  SlaveShMem->p_shs = sem_open(sem_name, 0, 0600);
  if(SlaveShMem->p_shs == SEM_FAILED)
   {
    // perror(" process: Error in shared memory semaphore.\n\r");
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

