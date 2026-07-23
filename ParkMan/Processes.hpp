#pragma once

#include "CommonData.h"
#include "main.hpp"
//#include "Processes.hpp"

#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <semaphore.h>
#include <string>


void GenShMemKeyID(key_t &sh_mem_key, int &sh_mem_id, void *&p_shm, size_t size);
void GenShSemKeyID(key_t &sh_sem_key, std::string &sem_name, sem_t *&p_shs);
void GenShQueName(std::string const &basic_name, std::string &que_name);


class ShSemMem_c
 {
  protected:
    key_t sh_mem_key = 0;      /* Task Control shared memroy key           */                                   // Is used as reference for other side process.
    int sh_mem_id = 0;         /* Task Control shared memroy ID            */       // Is used for removing.
    void *p_shm = nullptr;     /* Pointer to Task Control shared memory    */       // Is used for detatching.                                                   // Is used as reference for accessing.

    key_t sh_sem_key = 0;      /* Task Control shared semaphore key        */     
    sem_t *p_shs = nullptr;    /* Pointer to Task Control shared semaphore */                                                                                    // Is used as reference for accessing.
    std::string sem_name = ""; /* Task Control shared semaphore name       */       // Is used for unlinking.   // Is used for sharing by other side process.

    bool created = false;      /* "true" if the class is implemmented as the "Master" "false" if the class is used as "Slave". In case of master the shared memory and semaphore are created with generating their ID and name. In case of slave the shared memory and semaphore are only connected to the allready generated ID and name. After the usage the slave only disconnects from them while the master also distroys them. */
    bool ShMnc = false;        /* Shered memory not created    */
    bool ShSnc = false;        /* Shared Semaphore not created */
    void LoadShm(size_t size); /* In case of master it generates Memory key and creates the shared memory. In case of slave it connects to allready created shared memory according to given key. */
    void LoadShs();            /* In case of master it generates semaphore name and creates the semaphore itself. In case of slave it connects to allready created semaphore according to the given semaphore name. */
    void RemoveShm();          /* In case of master it disconnects from shared memory and distroys it. In case of slave it only disconnects from the shared memory. */
    void RemoveShs();          /* In case of master it unlinkes the semaphore. In case of slave it doesn't nothing. */

  public:
    ShSemMem_c(size_t size);
    ShSemMem_c(key_t sh_mem_key, const char sem_name[], size_t size);
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
    void SetDBFileName(std::string NameToSet);
    std::string GetDBFileName();
    void SetSHPFileName(std::string NameToSet);
    std::string GetSHPFileName();    
    void ReloadDatabase();
    bool DataBaseMustBeReloaded();  /* Is used from the slave side. Attention After reading the "DBUpdateRequired" flag it resets it immediately. So the result must be read at least and not more than one time. In case of multiusage the result must be saved in an emporary variable.*/    
 };







struct ProcParams_s
 {
  key_t sh_mem_key; 
  const char *sem_name; 
  ProcTypeID_e ProcType;
 };

typedef void(*subprocess_t)(key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType);

pid_t OpenProcess(subprocess_t ProcToOpen, ProcParams_s Procparams, char ProcName[]);


class Process_c: public TaskControl_ShSM_c
 {
     ProcTypeID_e proc_type;
    //  sem_t *p_shs;
    //  TskContShmData_s *p_shm;
     std::string proc_name;
    //  int sh_mem_id;
     bool exit_required;
     bool error_in_creation;           /* Is set to true in case of constructor couldn't initialize required variables. */
  public:
     Process_c(char ProcName[], key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType);
     virtual ~Process_c();
     void MakeExit();                  /* Breaks the default loop existing in the RunProcess. */
     virtual void RunProcess();        /* This procedure contains main loop with exit condition where is running the "OnRunProcess()" procedure, but can be overwritten according to requirements. */
     virtual void OnStartProcess();    /* This procedure is empty and runs before starting running the process for any initializations. */
     virtual void OnRunProcess();      /* This procedure contains the 1 second sleep and runs in the loop of the "RunProcess()" procedure, but can be overwritten. */
     virtual void OnFinishProcess();   /* This procedure is empty and runs after finishing running the process for any deinitializations. */
     virtual void CheckExitStatus();   /* This procedure contains the exit checking conditions and runs in the loop of the "RunProcess()" procedure, but can be overwritten. */
     Process_c& operator = (const Process_c &other) = delete;
     Process_c(const Process_c &other) = delete;
 };



 

 


