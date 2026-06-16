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
  pid_t proc_pid;
  proc_pid = fork();
  switch(proc_pid)
   {
    case -1:
      perror("fork error.");
      exit(EXIT_FAILURE);
     break;
    case 0:
      printf("Starting new process\n\r");
      ProcToOpen(sh_mem_key, sem_name);
      exit(EXIT_SUCCESS);
     break;
    default:
      printf("The new %s process with PID: %d started.\n\r", ProcName, proc_pid);
      return proc_pid;
     break;
   }
 }





/*======================================================================================================================*/


void GetShMemKeyID(key_t *sh_mem_key, int *sh_mem_id, void **p_shm, size_t size)
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

