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

class DataBase_c: public Process_c
 {
  public:
    DataBase_c(char ProcName[], key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType)
     :Process_c(ProcName, sh_mem_key, sem_name, ProcType)
     {};
    virtual ~DataBase_c(){};
    virtual void DoMainProg();
 };

void DataBaseProc(key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType)
 {
  DataBase_c DB_Process(DB_PROC_NAME, sh_mem_key, sem_name, ProcType);
  DB_Process.OnRunProcess();
 }



 void DataBase_c::DoMainProg()
  {
   Process_c::DoMainProg();
  };

