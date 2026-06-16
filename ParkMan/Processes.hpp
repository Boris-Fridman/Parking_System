#pragma once

#include "CommonData.h"
#include "main.hpp"

#include <sys/sem.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <string>


void GetShMemKeyID(key_t &sh_mem_key, int &sh_mem_id, void *&p_shm, size_t size);
void GenShSemKeyID(key_t &sh_sem_key, std::string &sem_name, sem_t *&p_shs);


class ShSemMem_c
 {
  protected:
    key_t sh_mem_key = 0;      /* Task Control shared memroy key           */                                   // Is used as reference for other side process.
    int sh_mem_id = 0;         /* Task Control shared memroy ID            */       // Is used for removing.
    void *p_shm = NULL;        /* Pointer to Task Control shared memory    */       // Is used for detatching.                                                   // Is used as reference for accessing.

    key_t sh_sem_key = 0;      /* Task Control shared semaphore key        */     
    sem_t *p_shs = NULL;       /* Pointer to Task Control shared semaphore */                                                                                    // Is used as reference for accessing.
    std::string sem_name = ""; /* Task Control shared semaphore name       */       // Is used for unlinking.   // Is used for sharing by other side process.

    bool created = false;
    bool ShMnc = false;        /* Shered memory not created    */
    bool ShSnc = false;        /* Shared Semaphore not created */
  public:
    ShSemMem_c(int size);
    ShSemMem_c(key_t sh_mem_key, const char sem_name[], int size);
    virtual ~ShSemMem_c();
    key_t ShMemKey();
    std::string SemName();
    ShSemMem_c& operator = (const ShSemMem_c &other) = delete;
    ShSemMem_c(const ShSemMem_c &other) = delete;
 };


/* Task Control Shared memory and Semaphore */
class TaskControl_ShSM_c:public ShSemMem_c
 {
  public:
    TaskControl_ShSM_c();
    TaskControl_ShSM_c(key_t sh_mem_key, const char sem_name[]);
    void ExitProcess(ProcTypeID_e ProcToExit);        /* Is used from the master process side. */
    void ExitAllProcesses();
    bool ProcessMustExit(ProcTypeID_e ProcToExit);    /* Is used from slaves processes sides.  */
 };







struct ProcParams_t
 {
  key_t sh_mem_key; 
  char *sem_name; 
  ProcTypeID_e ProcType;
 };

typedef void(*subprocess_t)(ProcParams_t Procparams, int count, ...);

pid_t OpenProcess(subprocess_t ProcToOpen, ProcParams_t Procparams, char ProcName[], int count, ...);


class Process_c
 {
     ProcTypeID_e proc_type;
     sem_t *p_shs;
     TskContShmData_s *p_shm;
     std::string proc_name;
     int sh_mem_id;
     bool exit_required;
     bool error_in_creation;           /* Is set to true in case of constructor couldn't initialize required variables. */
  public:
     Process_c(char ProcName[], key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType);
     virtual ~Process_c();
     void MakeExit();                  /* Breaks the default loop existing in the OnRunProcess. */
     virtual void OnRunProcess();      /* This procedure contains main loop with exit condition where is running the "DoMainProg()" procedure, but can be overwritten according to requirements. */
     virtual void DoMainProg();        /* This procedure contains the 1 second sleep and runs in the loop of the "OnRunProcess()" procedure, but can be overwritten. */
     virtual void CheckExitStatus();   /* This procedure contains the exit checking conditions and runs in the loop of the "OnRunProcess()" procedure, but can be overwritten. */
     Process_c& operator = (const Process_c &other) = delete;
     Process_c(const Process_c &other) = delete;
 };






