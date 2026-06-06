#include "Processes.hpp"


#include <iostream>
#include "Errors.hpp"

void OnStartProcess(ProcParam_s *MainParams, char ProcName[], key_t sh_mem_key, const char sem_name[])
 {
  std::cout << "Starting " << ProcName << " process...\n\rThe given sh_mem_key is: " << sh_mem_key << " and sem_name: " << sem_name << "\n\r";
  MainParams->sh_mem_id = shmget(sh_mem_key, SH_MEM_SIZE, 0666);
  if(MainParams->sh_mem_id == -1)
   {
    perr() << ProcName << " process: Error in shared memory.\n\r";
    return;
   }
  MainParams->p_shs = sem_open(sem_name, 0, 0600);
  if(MainParams->p_shs == SEM_FAILED)
   {
    perr() << ProcName << " process: Error in shared memory semaphore.\n\r";
    return;
   }
  MainParams->p_shm = (ShmData_s *)shmat(MainParams->sh_mem_id, NULL, 0);
 }



void OnEndProcess(ProcParam_s *MainParams, char ProcName[])
 {
  shmdt(MainParams->p_shm);
  std::cout << "Exitting from " << ProcName << " process... \n\r";
 }