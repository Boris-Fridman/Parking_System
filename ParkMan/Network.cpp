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


class Network_c: public Process_c
 {
  public:
    Network_c(char ProcName[], key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType)
     :Process_c(ProcName, sh_mem_key, sem_name, ProcType)
     {};
    virtual ~Network_c(){};
    virtual void DoMainProg();
 };


void NetworkProc(key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType)
 {
  Network_c Netw_Process(NETW_PROC_NAME, sh_mem_key, sem_name, ProcType);
  Netw_Process.OnRunProcess();
 }


void Network_c::DoMainProg()
 {
  Process_c::DoMainProg();
 };

