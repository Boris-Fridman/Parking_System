#pragma once

#include "CommonData.h"
#include "main.hpp"

#include <sys/sem.h>
#include <sys/shm.h>
#include <semaphore.h>

#define PROC_INIT_VAL {NULL, NULL, 0, false}
struct ProcParam_s
 {
  sem_t *p_shs;
  ShmData_s *p_shm;
  int sh_mem_id;
  bool exit_required;
 };

void OnStartProcess(ProcParam_s *MainParams, char ProcName[], key_t sh_mem_key, const char sem_name[]);

void OnEndProcess(ProcParam_s *MainParams, char ProcName[]);

