#include "Network.hpp"
#include "main.hpp"
#include "Errors.hpp"
#include "Processes.hpp"

#include <semaphore.h>
#include <unistd.h>
#include <iostream>
#include <sys/sem.h>
#include <sys/shm.h>

#define NETW_PROC_NAME     (char *)"Network"     /* Network process name*/

void NetworkProc(key_t sh_mem_key, const char sem_name[])
 {
//   sem_t *p_shs;
//   ShmData_s *p_shm;
//   int sh_mem_id;
//   bool exit_required = false;

  ProcParam_s MainParams = PROC_INIT_VAL;
  OnStartProcess(&MainParams , NETW_PROC_NAME, sh_mem_key, sem_name);


//   std::cout << "Starting Network process...\n\rThe given sh_mem_key is: " << sh_mem_key << " and sem_name: " << sem_name << "\n\r";    
//   sh_mem_id = shmget(sh_mem_key, SH_MEM_SIZE, 0666);
//   if(sh_mem_id == -1)
//    {
//     perr() << "Network process: Error in shared memory.\n\r";
//     return;
//    }
//   p_shs = sem_open(sem_name, 0, 0600);
//   if(p_shs == SEM_FAILED)
//    {
//     perr() << "Network process: Error in shared memory semaphore.\n\r";
//     return;
//    }
//   p_shm = (ShmData_s *)shmat(sh_mem_id, NULL, 0);

  //sleep(10);  // Was added for test only. In the future will be removed.

  do
   {
    sleep(1);
    MainParams.exit_required |= MainParams.p_shm->exit_network;
    MainParams.exit_required |= (getppid() == 1); // Checking if the parent process is running. If not enables exit.
   } 
  while (!MainParams.exit_required);


//   shmdt(p_shm);
//   std::cout << "Exitting from Network process... \n\r";

  OnEndProcess(&MainParams , NETW_PROC_NAME);  

 }