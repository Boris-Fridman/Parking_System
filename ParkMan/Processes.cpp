#include "Processes.hpp"


#include <iostream>
#include <unistd.h>
#include "Errors.hpp"





Process_c::Process_c(char ProcName[], key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType)
    : proc_type(ProcType), p_shs(NULL), p_shm(NULL), proc_name(ProcName), sh_mem_id(-1), exit_required(false), error_in_creation(false)
 {
  //proc_name = ProcName;
  std::cout << "Entering to " << proc_name << " process...\n\rThe given sh_mem_key is: " << sh_mem_key << " and sem_name: " << sem_name << "\n\r";
  sh_mem_id = shmget(sh_mem_key, SH_MEM_SIZE, 0666);
  if(sh_mem_id == -1)
   {
    perr() << ProcName << " process: Error in shared memory.\n\r";
    error_in_creation = true;
    return;
   }
  p_shs = sem_open(sem_name, 0, 0600);
  if(p_shs == SEM_FAILED)
   {
    perr() << proc_name << " process: Error in shared memory semaphore.\n\r";
    error_in_creation = true;
    return;
   }
  p_shm = (ShmData_s *)shmat(sh_mem_id, NULL, 0);
 }

Process_c::~Process_c()
 {
  if(p_shm != NULL)
   shmdt(p_shm);
  std::cout << "Exitting from " << proc_name << " process... \n\r";
 }

/* This procedure contains main loop with exit condition where is running the "DoMainProg()" procedure, but can be overwritten according to requirements. */
void Process_c::OnRunProcess()
 {
  while (!(exit_required || error_in_creation))
   {
    DoMainProg();
   }
  
 }

/* This procedure contains the exit checking conditions and runs in the loop of the "OnRunProcess()" procedure, but can be overwritten. */ 
void Process_c::DoMainProg()
 {
  sleep(1);
  exit_required |= p_shm->get_flag(proc_type);  //( p_shm->exit_proc_flags & (0x1 << proc_type) );     
  exit_required |= (getppid() == 1); // Checking if the parent process is running. If not enables exit.
 }




