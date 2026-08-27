#ifndef ____Processes_h__
#define ____Processes_h__

#include "CommonData.h"
#include "main.h"

//#include <stdarg.h> /* Required header for variadic processing. */
//#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <mqueue.h>
#include "Logging.h"



/*======================================================================================================================*/

/*----------------------------------------------------------------------------------------------------------------------*/
/* The structure with parameters required for control the interprocesses shared memory from the master side.            */
typedef struct MasterShMem_s
 {
  key_t sh_mem_key;          /* Task Control shared memroy key           */                              // Is used as reference for other side process.
  key_t sh_sem_key;          /* Task Control shared semaphore key        */
  int sh_mem_id;             /* Task Control shared memroy ID            */  // Is used for removing.
  sem_t *p_shs;              /* Pointer to Task Control shared semaphore */                                                                               // Is used as reference for accessing.
  void *p_shm;               /* Pointer to Task Control shared memory    */  // Is used for detatching.                                                   // Is used as reference for accessing.
  char sem_name[NAME_LEN];   /* Task Control shared semaphore name       */  // Is used for unlinking.   // Is used for sharing by other side process.
  
 }MasterShMem_s;

/*----------------------------------------------------------------------------------------------------------------------*/
/* The structure with parameters required for control the interprocesses shared memory from the slave side.             */
typedef struct SlaveShMem_s
 {
  sem_t *p_shs;
  void *p_shm;
  int sh_mem_id;
 }SlaveShMem_s;


/*----------------------------------------------------------------------------------------------------------------------*/
/* The structure with parameters required for control the interprocesses shared queue from the master side.             */
typedef struct MasterShQue_s
 {
  char sq_name[PATH_LEN];    /* Shared Queue name                        */
  key_t sh_sem_key;          /* Task Control shared semaphore key        */
  mqd_t mq;                  /* Shared Queue variable                    */
  char sem_name[NAME_LEN];   /* Task Control shared semaphore name       */  // Is used for unlinking.   // Is used for sharing by other side process.
  sem_t *p_shs;              /* Pointer to Task Control shared semaphore */                                                                               // Is used as reference for accessing.
 }MasterShQue_s;

/*----------------------------------------------------------------------------------------------------------------------*/
/* The structure with parameters required for control the interprocesses shared queue from the slave side.              */
typedef struct SlaveShQue_s
 {
  char sq_name[PATH_LEN];    /* Shared Queue name                        */
  mqd_t mq;                  /* Shared Queue variable                    */
  char sem_name[NAME_LEN];   /* Task Control shared semaphore name       */  // Is used for unlinking.   // Is used for sharing by other side process.
  sem_t *p_shs;
 }SlaveShQue_s;


/*----------------------------------------------------------------------------------------------------------------------*/
/* The structure with parameters required for accessing the logging queue.                                              */
typedef struct LogData_s
 {
  pthread_t LogThread;
  LogParams_s LogParams;
  mqd_t *p_sq;
  bool Exit;
 }LogData_s;

/*----------------------------------------------------------------------------------------------------------------------*/
/* The structure with parameters for using the initilized shared memory between processes.                              */
typedef struct TaskSMBriefParams_s
 {
  void *p_shm;
  sem_t *p_shs;
 }TaskSMBriefParams_s;

/*----------------------------------------------------------------------------------------------------------------------*/
/* The structure with parameters for using the initilized shared loggig queue.                                          */
typedef struct LogSQBriefParams_s
 {
  mqd_t mq;
  sem_t *p_shs;
 }LogSQBriefParams_s;

/*----------------------------------------------------------------------------------------------------------------------*/
/* The structure with parameters for the additional processes.                                                          */
typedef struct ProcParams_s
 {
  char *ProcName;
  TaskSMBriefParams_s TskContShms;
  LogSQBriefParams_s TskContShqs;
 }ProcParams_s;


/*======================================================================================================================*/

typedef void(*SubProcess_t)(ProcParams_s *ProcParams);

/*======================================================================================================================*/

void set_flag(TskContShmData_s *TskContShmData, ProcTypeID_e flagno, bool state);
bool get_flag(TskContShmData_s *TskContShmData, ProcTypeID_e flagno);

/*======================================================================================================================*/

/**
 * @brief Opens a new process. 
 * 
 * @code
 * pid_t OpenProcess(SubProcess_t ProcToOpen, char ProcName[], key_t sh_mem_key, char msem_name[], char sh_que_name[], char qsem_name[]);
 * @code
 * 
 * @param ProcToOpen The pointer to process-defining-function.
 * 
 * @param ProcName The name given to the process.
 * 
 * @param sh_mem_key Shared memory key for process usage.
 * 
 * @param msem_name Shared semaphore name for using with shared memory to which is referred the parameter "sh_mem_key".
 * 
 * @param sh_que_name Shared queue for sending log messages.
 * 
 * @param qsem_name Shared semaphore name for usage with shared queue to which is referred the parameter "sh_que_name".
 * 
 * @return Process ID.
 */

pid_t OpenProcess(SubProcess_t ProcToOpen, char ProcName[], key_t sh_mem_key, char msem_name[], char sh_que_name[], char qsem_name[]);

/*======================================================================================================================*/

/**
 * @brief Generatesa a unique shared memory key not existing yet in the operating system
 *        and creates a shared memory with the generated key.
 * 
 * @code
 * void GenShMemKeyID(key_t *sh_mem_key, int *sh_mem_id, void **p_shm, size_t size);
 * @code
 * 
 * @param sh_mem_key The returned shared memory key generated for control.
 * 
 * @param sh_mem_id The returned shared memory ID for controlling the shared memory.
 * 
 * @param p_shm The returned pointer to new-created shared memory.
 * 
 * @param size The given shared memory size in which the memory must be generated.
 */
void GenShMemKeyID(key_t *sh_mem_key, int *sh_mem_id, void **p_shm, size_t size);

/**
 * @brief Generatesa a unique shared semaphore key not existing yet in the operating system
 *        and creates a shered semaphore with the generated key.
 * 
 * @code
 * void GenShSemKeyID(key_t *sh_sem_key, char sem_name[], sem_t **p_shs);
 * @code
 * 
 * @param sh_sem_key The returned shared semaphore key generated for control.
 * 
 * @param sem_name The returned shared semaphore name for control.
 * 
 * @param p_shs The returned pointer to the shared semaphore for usate.
 */
void GenShSemKeyID(key_t *sh_sem_key, char sem_name[], sem_t **p_shs);

/**
 * @brief Generatesa a unique shared queue name not existing yet in the operating system
 *        and creates a shered queue with the generated name.
 * 
 * @code 
 * void GenShQueName(char const basic_name[], char que_name[], size_t MaxSize);
 * @code
 * 
 * @param basic_name The given Basic name of the queue from which must be generated the final name with coded suffix.
 * 
 * @param que_name The retuned Final name of the queue generated from the basic name with coded suffix.
 * 
 * @param MaxSize The given maximal permited length of final generated name of the queue. If the given length is smaller that required the part of the data will be lost.
 */
void GenShQueName(char const basic_name[], char que_name[], size_t MaxSize);

/*----------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Activate shared memory including its semaphore from the master side thats means with creation.
 * 
 * @code
 * void ActivateMasterShMem(MasterShMem_s *MasterShMem, int size);
 * @code
 * 
 * @param MasterShMem The retrned structure containing the parameters of the activated shared memory from the "Master" side.
 * 
 * @param size The shared memory size.
 */
void ActivateMasterShMem(MasterShMem_s *MasterShMem, int size);

/**
 * @brief Deactivates shared memory including its semaphore from the master side.
 *        
 * @code
 * void DeactivateMasterShMem(MasterShMem_s *MasterShMem);
 * @code
 * 
 * @param MasterShMem The given for modification structure of the shared memory parameters from the "Master" side.
 */
void DeactivateMasterShMem(MasterShMem_s *MasterShMem);


/**
 * @brief Activate shared memory including its semaphore from the slave side thats means with connection.
 * 
 * @code
 * void ActivateSlaveShMem(SlaveShMem_s *SlaveShMem, key_t sh_mem_key, const char sem_name[], int size);
 * @code
 * 
 * @param SlaveShMem The returned structure containing the parameters required for accessing the shared memory from the "Slave" side.
 * 
 * @param sh_mem_key The given shared memory key for accessing it.
 * 
 * @param sem_name   The given name of the semaphore ensuring the safe access to the shared memory.
 * 
 * @param size       The given size of the shared memory.
 */
void ActivateSlaveShMem(SlaveShMem_s *SlaveShMem, key_t sh_mem_key, const char sem_name[], int size);

/**
 * @brief Deactivates shared memory including its semaphore from the slave side.
 * 
 * @code
 * void DeactivateSlaveShMem(SlaveShMem_s *SlaveShMem);
 * @code
 * 
 * @param SlaveShMem The returned structure containing the parameters required for accessing the shared memory from the "Slave" side.
 */
void DeactivateSlaveShMem(SlaveShMem_s *SlaveShMem);

/*----------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Activates shared queue including its semaphore from the master side thats means with creation.
 * 
 * @code
 * void ActivateMasterShQue(MasterShQue_s *MasterShQue, QueueDirection_e const SendReceive, char const basic_name[], long int msg_size);
 * @code
 * 
 * @param MasterShMem The updated structure containing the parameters of the activated shared memory from the "Master" side.
 * 
 * @param SendReceive The shared queue direction for sending, receiving or sending and receiving the messates.
 * 
 * @param basic_name  The given Basic name of the queue from which must be generated the final name with coded suffix.
 * 
 * @param msg_size    The given size of the message sent / received via the queue.
 */
void ActivateMasterShQue(MasterShQue_s *MasterShQue, QueueDirection_e const SendReceive, char const basic_name[], long int msg_size);

/**
 * @brief Deactivates shared queue including its semaphore from the master side.
 * 
 * @code
 * void DeactivateMasterShQue(MasterShQue_s *MasterShQue);
 * @code
 * 
 * @param MasterShMem The updated structure containing the parameters for deactivating the shared memory from the "Master" side.
 */
void DeactivateMasterShQue(MasterShQue_s *MasterShQue);

/**
 * @brief Activates shared queue including its semaphore from the master side thats means with creation.
 * 
 * @code
 * void ActivateSlaveShQue(SlaveShQue_s *SlaveShQue, char sh_que_name[], char qsem_name[], QueueDirection_e const SendReceive, long int const msg_size);
 * @code
 * 
 * @param SlaveShQue  The updated structure containing the parameters of the activated shared memory from the "Slave" side.
 * 
 * @param sh_que_name The given name of the queue shared queue.
 * 
 * @param qsem_name   The name of the semaphore ensuritng the safe access to the shared queu.
 * 
 * @param SendReceive The shared queue direction for sending, receiving or sending and receiving the messates.
 * 
 * @param msg_size    The given size of the message sent / received via the queue.
 */
void ActivateSlaveShQue(SlaveShQue_s *SlaveShQue, char sh_que_name[], char qsem_name[], QueueDirection_e const SendReceive, long int const msg_size);

/**
 * @brief Deactivates shared queue including its semaphore from the slave side.
 * 
 * @code
 * void DeactivateSlaveShQue(SlaveShQue_s *SlaveShQue);
 * @code
 * 
 * @param SlaveShQue  The updated structure containing the parameters given for deactivated the shared memory from the "Slave" side.
 */
void DeactivateSlaveShQue(SlaveShQue_s *SlaveShQue);

/*----------------------------------------------------------------------------------------------------------------------*/

/**
 * @brief Initilizes shared queue from master or slave side to sending or receiving accroding requirements.
 * 
 * @code
 * void InitQueue(mqd_t *mq, QueueDirection_e SendReceive, char const que_name[], long int const msg_size);
 * @code
 * 
 * @param mq          The returned pointer to the queue to be activated.
 * 
 * @param SendReceive The shared queue direction for sending, receiving or sending and receiving the messates.
 * 
 * @param que_name    The given name of the shared queue to be initilized.
 * 
 * @param msg_size    The size of the message to be sent or received.
 */
void InitQueue(mqd_t *mq, QueueDirection_e SendReceive, char const que_name[], long int const msg_size);

/**
 * @brief Closes shared queue from master or slave side.
 * 
 * @code
 * void CloseQueue(mqd_t *mq, char const que_name[]);
 * @code
 * 
 * @param mq          The given pointer to the queue to be closed.
 * 
 * @param que_name    The given name of the shared queue to be closed.
 */
void CloseQueue(mqd_t *mq, char const que_name[]);

/*======================================================================================================================*/

/**
 * @brief Initilizes process managing.
 * 
 * @code
 * void InitManaging(MasterShMem_s *TskContShms, MasterShQue_s *TskContShqs, LogData_s *LogData);
 * @code
 * 
 * @param TskContShms Task Control shared memory parameters (including semaphore).
 * 
 * @param TskContShqs Task Control shared queue parameters (including semaphore).
 * 
 * @param LogData     Parameters for logging.
 */
void InitManaging(MasterShMem_s *TskContShms, MasterShQue_s *TskContShqs, LogData_s *LogData);

/**
 * @brief Deactivates shared queue including its semaphore from the master side.
 * 
 * @code
 * void DeinitManaging(MasterShMem_s *TskContShms, MasterShQue_s *TskContShqs, LogData_s *LogData);
 * @code
 * 
 * @param TskContShms Task Control shared memory parameters (including semaphore).
 * 
 * @param TskContShqs Task Control shared queue parameters (including semaphore).
 * 
 * @param LogData     Parameters for logging.
 */
void DeinitManaging(MasterShMem_s *TskContShms, MasterShQue_s *TskContShqs, LogData_s *LogData);

/*======================================================================================================================*/

/**
 * @brief Initilizes the required data from processes' side.
 * 
 * @code
 * void InitProcessing(SlaveShMem_s *TskContShms, SlaveShQue_s *TskContShqs, key_t sh_mem_key, const char sem_name[], char sh_que_name[], char qsem_name[]);
 * @code
 * 
 * @param TskContShms Task Control shared memory parameters (including semaphore).
 * 
 * @param TskContShqs Task Control shared queue parameters (including semaphore).
 * 
 * @param sh_mem_key Shared memory key for process usage.
 * 
 * @param msem_name Shared semaphore name for using with shared memory to which is referred the parameter "sh_mem_key".
 * 
 * @param sh_que_name Shared queue for sending log messages.
 * 
 * @param qsem_name Shared semaphore name for usage with shared queue to which is referred the parameter "sh_que_name".
 */
void InitProcessing(SlaveShMem_s *TskContShms, SlaveShQue_s *TskContShqs, key_t sh_mem_key, const char msem_name[], char sh_que_name[], char qsem_name[]);

/**
 * @brief Deactivates shared queue including its semaphore from the master side.
 * 
 * @code
 * void DeinitProcessing(SlaveShMem_s *TskContShms, SlaveShQue_s *TskContShqs);
 * @code
 * 
 * @param TskContShms Task Control shared memory parameters (including semaphore).
 * 
 * @param TskContShqs Task Control shared queue parameters (including semaphore).
 */
void DeinitProcessing(SlaveShMem_s *TskContShms, SlaveShQue_s *TskContShqs);

/*======================================================================================================================*/

/**
 * @brief Receives the logging information from processes and sends it to the logging queue.
 * 
 * @code
 * void LogEvent(LogSQBriefParams_s *LogQueueParams, LogMessType_s MessageToLog);
 * @code
 * 
 * @param LogQueueParams The log queue parameters.
 * 
 * @param MessageToLog   The message to be logged.
 */
void LogEvent(LogSQBriefParams_s *LogQueueParams, LogMessType_s MessageToLog);

#endif  // ____Processes_h__

/*======================================================================================================================*/
