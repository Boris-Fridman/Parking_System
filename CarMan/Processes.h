#ifndef ____Processes_h__
#define ____Processes_h__

#include "CommonData.h"
#include "main.h"

//#include <stdarg.h> /* Required header for variadic processing. */
//#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <semaphore.h>






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


typedef void(*subprocess_t)(key_t sh_mem_key, char sem_name[]);



void set_flag(TskContShmData_s *TskContShmData, ProcTypeID_e flagno, bool state);
bool get_flag(TskContShmData_s *TskContShmData, ProcTypeID_e flagno);

pid_t OpenProcess(subprocess_t, char ProcName[], key_t sh_mem_key, char sem_name[]);

void GetShMemKeyID(key_t *sh_mem_key, int *sh_mem_id, void **p_shm, size_t size);
void GenShSemKeyID(key_t *sh_sem_key, char sem_name[], sem_t **p_shs);

void ActivateSlaveShMem(SlaveShMem_s *SlaveShMem, key_t sh_mem_key, const char sem_name[], int size);
void DeactivateSlaveShMem(SlaveShMem_s *SlaveShMem);


#endif  // ____Processes_h__

