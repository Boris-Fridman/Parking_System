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






typedef struct MasterShMem_s
 {
  key_t sh_mem_key;          /* Task Control shared memroy key           */                              // Is used as reference for other side process.
  key_t sh_sem_key;          /* Task Control shared semaphore key        */
  int sh_mem_id;             /* Task Control shared memroy ID            */  // Is used for removing.
  sem_t *p_shs;              /* Pointer to Task Control shared semaphore */                                                                               // Is used as reference for accessing.
  void *p_shm;               /* Pointer to Task Control shared memory    */  // Is used for detatching.                                                   // Is used as reference for accessing.
  char sem_name[NAME_LEN];   /* Task Control shared semaphore name       */  // Is used for unlinking.   // Is used for sharing by other side process.
  
 }MasterShMem_s;


typedef struct SlaveShMem_s
 {
  sem_t *p_shs;
  void *p_shm;
  int sh_mem_id;
 }SlaveShMem_s;


typedef struct MasterShQue_s
 {
  char sq_name[PATH_LEN];    /* Shared Queue name                        */
  key_t sh_sem_key;          /* Task Control shared semaphore key        */
  mqd_t mq;                  /* Shared Queue variable                    */
  char sem_name[NAME_LEN];   /* Task Control shared semaphore name       */  // Is used for unlinking.   // Is used for sharing by other side process.
  sem_t *p_shs;              /* Pointer to Task Control shared semaphore */                                                                               // Is used as reference for accessing.
 }MasterShQue_s;

typedef struct SlaveShQue_s
 {
  char sq_name[PATH_LEN];    /* Shared Queue name                        */
  mqd_t mq;                  /* Shared Queue variable                    */
  char sem_name[NAME_LEN];   /* Task Control shared semaphore name       */  // Is used for unlinking.   // Is used for sharing by other side process.
  sem_t *p_shs;
 }SlaveShQue_s;


typedef struct LogData_s
 {
  pthread_t LogTHread;
  LogParams_s LogParams;
  mqd_t *p_sq;
  bool Exit;
 }LogData_s;

typedef struct TaskSMBriefParams_s
 {
  void *p_shm;
  sem_t *p_shs;
 }TaskSMBriefParams_s;

typedef struct LogSQBriefParams_s
 {
  mqd_t mq;
  sem_t *p_shs;
 }LogSQBriefParams_s;



typedef void(*subprocess_t)(SlaveShMem_s *TskContShms, SlaveShQue_s *TskContShqs);



void set_flag(TskContShmData_s *TskContShmData, ProcTypeID_e flagno, bool state);
bool get_flag(TskContShmData_s *TskContShmData, ProcTypeID_e flagno);

pid_t OpenProcess(subprocess_t, char ProcName[], key_t sh_mem_key, char sem_name[], char sh_que_name[], char qsem_name[]);

void GenShMemKeyID(key_t *sh_mem_key, int *sh_mem_id, void **p_shm, size_t size);
void GenShSemKeyID(key_t *sh_sem_key, char sem_name[], sem_t **p_shs);

void ActivateMasterShMem(MasterShMem_s *MasterShMem, int size);
void DeactivateMasterShMem(MasterShMem_s *MasterShMem);

void ActivateSlaveShMem(SlaveShMem_s *SlaveShMem, key_t sh_mem_key, const char sem_name[], int size);
void DeactivateSlaveShMem(SlaveShMem_s *SlaveShMem);



void ActivateMasterShQue(MasterShQue_s *MasterShQue, QueueDirection_e const SendReceive, char const basic_name[], long int msg_size);
void DeactivateMasterShQue(MasterShQue_s *MasterShQue);

void ActivateSlaveShQue(SlaveShQue_s *SlaveShQue, char sh_que_name[], char qsem_name[], QueueDirection_e const SendReceive, long int const msg_size);
void DeactivateSlaveShQue(SlaveShQue_s *SlaveShQue);





void InitQueue(mqd_t *mq, QueueDirection_e SendReceive, char const que_name[], long int const msg_size);
void CloseQueue(mqd_t *mq, char const que_name[]);




void InitManaging(MasterShMem_s *TskContShms, MasterShQue_s *TskContShqs, LogData_s *LogData);
void DeinitManaging(MasterShMem_s *TskContShms, MasterShQue_s *TskContShqs, LogData_s *LogData);


void InitProcessing(SlaveShMem_s *TskContShms, SlaveShQue_s *TskContShqs, key_t sh_mem_key, const char sem_name[], char sh_que_name[], char qsem_name[]);
void DeinitProcessing(SlaveShMem_s *TskContShms, SlaveShQue_s *TskContShqs);


LogSQBriefParams_s LgSlToLgPars(SlaveShQue_s *LogQueueParams);  /* Log Slave queue&semaphore params to breaf Log params. */
LogSQBriefParams_s LgMsToLgPars(MasterShQue_s *LogQueueParams);  /* Log Master queue&semaphore params to breaf Log params. */


void LogEvent(LogSQBriefParams_s *LogQueueParams, LogMessType_s MessageToLog);

#endif  // ____Processes_h__

