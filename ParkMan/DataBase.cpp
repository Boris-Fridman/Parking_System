#include "DataBase.hpp"
#include "main.hpp"
#include "Errors.hpp"
#include "Processes.hpp"

#include <semaphore.h>
#include <unistd.h>
#include <iostream>
#include <sys/sem.h>
#include <sys/shm.h>

#define DB_PROC_NAME     (char *)"DataBase"     /* Database process name*/


void DataBaseProc(key_t sh_mem_key, const char sem_name[])
 {
  ProcParam_s MainParams = PROC_INIT_VAL;
//   sem_t *p_shs;
//   ShmData_s *p_shm;
//   int sh_mem_id;
//   bool exit_required = false;

  OnStartProcess(&MainParams , DB_PROC_NAME, sh_mem_key, sem_name);

//   std::cout << "Starting DataBase process...\n\rThe given sh_mem_key is: " << sh_mem_key << " and sem_name: " << sem_name << "\n\r";
//   sh_mem_id = shmget(sh_mem_key, SH_MEM_SIZE, 0666);
//   if(sh_mem_id == -1)
//    {
//     perr() << "DataBase process: Error in shared memory.\n\r";
//     return;
//    }
//   p_shs = sem_open(sem_name, 0, 0600);
//   if(p_shs == SEM_FAILED)
//    {
//     perr() << "DataBase process: Error in shared memory semaphore.\n\r";
//     return;
//    }
//   p_shm = (ShmData_s *)shmat(sh_mem_id, NULL, 0);
  

  //sleep(10);  // Was added for test only. In the future will be removed.

  do
   {
    sleep(1);
    MainParams.exit_required |= MainParams.p_shm->exit_database;     
    MainParams.exit_required |= (getppid() == 1); // Checking if the parent process is running. If not enables exit.
   }
  while (!MainParams.exit_required);
  

//   shmdt(p_shm);
//   std::cout << "Exitting from DataBase process... \n\r";


  OnEndProcess(&MainParams , DB_PROC_NAME);

 }

