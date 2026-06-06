#pragma once

#include "CommonData.h"
#include "main.hpp"

#include <sys/sem.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <string>

#define PROC_INIT_VAL {NULL, NULL, 0, false}





struct ProcParam_s
 {
  sem_t *p_shs;
  ShmData_s *p_shm;
  int sh_mem_id;
  bool exit_required;
 };



class Process_c
 {
     sem_t *p_shs;
     ShmData_s *p_shm;
     std::string proc_name;
     int sh_mem_id;
     bool exit_required;
  public:
     Process_c(char ProcName[], key_t sh_mem_key, const char sem_name[]);
     virtual ~Process_c();
     virtual void OnRunProcess();  /* This procedure contains main loop with exit condition where is running the "DoMainProg()" procedure, but can be overwritten according to requirements. */
     virtual void DoMainProg();    /* This procedure contains the exit checking conditions and runs in the loop of the "OnRunProcess()" procedure, but can be overwritten. */
     Process_c& operator = (const Process_c& other) = delete;
     Process_c(const Process_c& oter) = delete;
 };











// void OnStartProcess(ProcParam_s *MainParams, char ProcName[], key_t sh_mem_key, const char sem_name[]);

// void OnEndProcess(ProcParam_s *MainParams, char ProcName[]);

