#pragma once

#include "CommonData.h"
#include "main.hpp"

#include <sys/sem.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <string>


struct ProcParams_t
 {
  key_t sh_mem_key; 
  char *sem_name; 
  ProcTypeID_e ProcType;
 };

typedef void(*subprocess_t)(ProcParams_t Procparams);

pid_t OpenProcess(subprocess_t ProcToOpen, ProcParams_t Procparams, char ProcName[]);


class Process_c
 {
     ProcTypeID_e proc_type;
     sem_t *p_shs;
     ShmData_s *p_shm;
     std::string proc_name;
     int sh_mem_id;
     bool exit_required;
     bool error_in_creation;  // Is set to true in case of constructor couldn't initialize required variables.
  public:
     Process_c(char ProcName[], key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType);
     virtual ~Process_c();
     void MakeExit();                  /* Breaks the default loop existing in the OnRunProcess. */
     virtual void OnRunProcess();      /* This procedure contains main loop with exit condition where is running the "DoMainProg()" procedure, but can be overwritten according to requirements. */
     virtual void DoMainProg();        /* This procedure contains the 1 second sleep and runs in the loop of the "OnRunProcess()" procedure, but can be overwritten. */
     virtual void CheckExitStatus();   /* This procedure contains the exit checking conditions and runs in the loop of the "OnRunProcess()" procedure, but can be overwritten. */
     Process_c& operator = (const Process_c &other) = delete;
     Process_c(const Process_c &oter) = delete;
 };






