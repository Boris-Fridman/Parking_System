#pragma once

/*======================================================================================================================*/

#include "CommonData.h"
#include "main.hpp"
#include "Logging.h"

#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <semaphore.h>
#include <string>


/*======================================================================================================================*/

void GenShMemKeyID(key_t &sh_mem_key, int &sh_mem_id, void *&p_shm, size_t size);
void GenShSemKeyID(key_t &sh_sem_key, std::string &sem_name, sem_t *&p_shs);
void GenShQueName(std::string const &basic_name, std::string &que_name);

/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Shared memory and semaphores class.
 * *************************************************************************************************************
 */

class ShSemMem_c
 {
  protected:
    /* Task Control shared memory variables */
    key_t sh_mem_key = 0;      /* Task Control shared memroy key           */                                   // Is used as reference for other side process.
    int sh_mem_id = 0;         /* Task Control shared memroy ID            */       // Is used for removing.
    void *p_shm = nullptr;     /* Pointer to Task Control shared memory    */       // Is used for detatching.                                                   // Is used as reference for accessing.

    /* Task Congrol shared queue semaphore variables */
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
    ShSemMem_c(size_t size);                                           /* Constructor for initialization the shared memory from the master side. */
    ShSemMem_c(key_t sh_mem_key, const char sem_name[], size_t size);  /* Constructor for initialization the shared memory from the slave side.  */
    virtual ~ShSemMem_c();                                             /* Destructor for deinitialization the shared memory.                     */
    key_t ShMemKey();                                                  /* Returns the shared memory key containing in the class.                 */
    std::string &SemName();                                            /* Returns the shared-memory-semaphore-name containgint in this class.    */
    ShSemMem_c& operator = (const ShSemMem_c &other) = delete;
    ShSemMem_c(const ShSemMem_c &other) = delete;
 };

/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Shared memory, queue and semaphores class.
 * *************************************************************************************************************
 */

class ShSemMemQue_c:public ShSemMem_c
 {
  protected:
    /* Logging shared queue variables */
    std::string sq_name = "";   /* Shared Queue name */
    mqd_t p_sq = 0;             /* Shared Queue variable */
    
    /* Logging shared queue semaphore variables */
    key_t sh_qsem_key = 0;      /* Queue shared semaphore key        */     
    sem_t *p_shqs = nullptr;    /* Pointer to Queue shared semaphore */
    std::string qsem_name = ""; /* Queue shared semaphore name       */

    void LoadShq(QueueDirection_e SendReceive, std::string const basic_name, size_t size); /* In case of master it generates Memory key and creates the shared memory. In case of slave it connects to allready created shared memory according to given key. */
    void LoadShqs();                                                                       /* In case of master it generates semaphore name and creates the semaphore itself. In case of slave it connects to allready created semaphore according to the given semaphore name. */
    void RemoveShq();                                                                      /* In case of master it disconnects from shared memory and distroys it. In case of slave it only disconnects from the shared memory. */
    void RemoveShqs();                                                                     /* In case of master it unlinkes the semaphore. In case of slave it doesn't nothing. */
  public:
    ShSemMemQue_c(size_t shmem_size, QueueDirection_e queue_direction, std::string queu_basic_name, size_t shque_size);                                                         /* Constructor for initialization the shared memory-queue from the master side. */
    ShSemMemQue_c(key_t sh_mem_key, const char sem_name[], std::string sq_name, std::string qsem_name, size_t shmem_size, QueueDirection_e queue_direction, size_t shque_size); /* Constructor for initialization the shared memory-queue from the slave side.  */
    ~ShSemMemQue_c();                                                                                                                                                           /* Destructor for deinitialization the shared memory-queue.                     */
    ShSemMemQue_c& operator = (const ShSemMemQue_c &other) = delete;
    ShSemMemQue_c(const ShSemMemQue_c &other) = delete;
    std::string &QueueName();                                                                                                                                                   /* Returns the shared queue name containing in the class.                        */
    std::string &QSemName();                                                                                                                                                    /* Returns the shared-queue-semaphore-name containing in the class.              */
    
 };

/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Task Control Shared memory and Semaphore class.
 * *************************************************************************************************************
 */

/* Task Control Shared memory and Semaphore */
class TaskControl_ShSM_c:public ShSemMemQue_c
 {
  public:
    TaskControl_ShSM_c();                                                                                     /* Constructor for initialization of the task class from the master side.                  */
    TaskControl_ShSM_c(key_t sh_mem_key, const char sem_name[], std::string sq_name, std::string qsem_name);  /* Constructor for initialization of the task class from the skave side.                   */
    void ExitProcess(ProcTypeID_e ProcToExit);        /* Is used from the master process side. */             /* The command for exitting the given process.                                             */
    void ExitAllProcesses();                                                                                  /* Command for exitting all processes.                                                     */
    bool ProcessMustExit(ProcTypeID_e ProcToExit);    /* Is used from slaves processes sides.  */             /* Function for checking the flag of the process according the given type if it must exit. */
    void SetDBFileName(std::string NameToSet);                                                                /* Sets the path and name of the database file in the class.                               */
    std::string &GetDBFileName();                                                                             /* Returns the path and name of the atabase file written in the class.                     */
    void SetSHPFileName(std::string NameToSet);                                                               /* Sets the path and name to the shape file in the class.                                  */
    std::string &GetSHPFileName();                                                                            /* Returns the path and name of the shape file witten in the class.                        */
    void ReloadDatabase();                                                                                    /* Gives command to update the database loaded in the shared memory from the master side.  */
    bool DataBaseMustBeReloaded();  /* Is used from the slave side. Attention After reading the "DBUpdateRequired" flag it resets it immediately. So the result must be read at least and not more than one time. In case of multiusage the result must be saved in an emporary variable.*/    
    void LogEvent(LogMessType_s MessageToLog);                                                                /* Receives the logging information from processes and sends it to the logging queue.      */
 };



/*======================================================================================================================*/

/*----------------------------------------------------------------------------------------------------------------------*/
/* The structure with parameters for the additional processes.                                                          */

struct ProcParams_s
 {
  key_t sh_mem_key; 
  const char *sem_name; 
  const char *sq_name;
  const char *qsem_name;
  ProcTypeID_e ProcType;
 };

 /*======================================================================================================================*/

typedef void(*SubProcess_t)(key_t sh_mem_key, const char sem_name[], std::string sq_name, std::string qsem_name, ProcTypeID_e ProcType);

/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Process managing class.
 * *************************************************************************************************************
 */

class Process_c: public TaskControl_ShSM_c
 {
     ProcTypeID_e proc_type;
     std::string proc_name;
     bool exit_required;
     bool error_in_creation;           /* Is set to true in case of constructor couldn't initialize required variables. */
  public:
     Process_c(char ProcName[], key_t sh_mem_key, const char sem_name[], std::string sq_name, std::string qsem_name, ProcTypeID_e ProcType); /* Constructor for initilizing the "Process" class.  */
     virtual ~Process_c();                                                                                                                   /* Destructor for deinitilizing the "Process" class. */
     void MakeExit();                  /* Breaks the default loop existing in the RunProcess. */
     virtual void RunProcess();        /* This procedure contains main loop with exit condition where is running the "OnRunProcess()" procedure, but can be overwritten according to requirements. */
  protected:
     virtual void OnStartProcess();    /* This procedure is empty and runs before starting running the process for any initializations. */
     virtual void OnRunProcess();      /* This procedure contains the 1 second sleep and runs in the loop of the "RunProcess()" procedure, but can be overwritten. */
     virtual void OnFinishProcess();   /* This procedure is empty and runs after finishing running the process for any deinitializations. */
     virtual void CheckExitStatus();   /* This procedure contains the exit checking conditions and runs in the loop of the "RunProcess()" procedure, but can be overwritten. */
  public:
     Process_c& operator = (const Process_c &other) = delete;
     Process_c(const Process_c &other) = delete;
     std::string &GetProcName();       /* Returns the name of process stored in the class. */
 };

/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Process controlling parameters class.
 * *************************************************************************************************************
 */

class ProcMan_c: public TaskControl_ShSM_c
 {
    pthread_t LogTHread = 0;
    bool Exit = false;
    LogParams_s LogParams;
    void *LogThread(void *Args);            /* The procedure defining the logging thread that receives messages from the logging queue and writes them to the file. */
    void static *StatLogThread(void *Args); /* Starts logging thread.                                                                                               */
  public:
    ProcMan_c();                            /* Constructor for initilizing the "ProcMan_c" class.                                                                   */
    virtual ~ProcMan_c();                   /* Destructor for deinitilizing the "ProcMan_c" class.                                                                  */
    void CheckLogMessageExistance();        /* Checks the logging message existance in the logging queue.                                                           */
    void LoadLogThread();                   /* Creates the logging thread that creceives log messages from queue.                                                   */
    void CloseLogThread();                  /* Sets exit flag to "true" value letting the logging thread to exit.                                                   */
 };
 
/*======================================================================================================================*/

/**
 * @brief Opens a new process. 
 * 
 * @code
 * pid_t OpenProcess(SubProcess_t ProcToOpen, ProcParams_s Procparams, char ProcName[], ProcMan_c *TaskControl = nullptr);
 * @code
 * 
 * @param ProcToOpen The pointer to process-defining-function.
 * 
 * @param Procparams The parameters given to process for usage containing the shared memory and semaphore between processea controlling flags and other optional possible parameters.
 * 
 * @param ProcName The name given to the process.
 * 
 * @param TaskControl The Process-Controlling-Class-Object.
 * 
 * @return Process ID.
 */
pid_t OpenProcess(SubProcess_t ProcToOpen, ProcParams_s Procparams, char ProcName[], ProcMan_c *TaskControl = nullptr);


/*======================================================================================================================*/
