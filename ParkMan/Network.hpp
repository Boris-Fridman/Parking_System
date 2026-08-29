#pragma once

/*======================================================================================================================*/

#include "CommonData.h"
#include "main.hpp"

#include <sys/types.h>

/*======================================================================================================================*/

#define NETW_PROC_NAME     (char *)"Network "     /* Network process name*/

/*======================================================================================================================*/

/**
 * @brief Main Process' operating procedure.
 * 
 * @code
 * void DataBaseProc(key_t sh_mem_key, const char sem_name[], std::string sq_name, std::string qsem_name, ProcTypeID_e ProcType);
 * @code
 * 
 * @param sh_mem_key  Shared memory key for process usage.
 * 
 * @param msem_name   Shared semaphore name for using with shared memory to which is referred the parameter "sh_mem_key".
 * 
 * @param sh_que_name Shared queue for sending log messages.
 * 
 * @param qsem_name   Shared semaphore name for usage with shared queue to which is referred the parameter "sh_que_name".
 * 
 * @param ProcType    The enumeration containing the enumerated types of the additional processes.
 */
void NetworkProc(key_t sh_mem_key, const char msem_name[], std::string sq_name, std::string qsem_name, ProcTypeID_e ProcType);

/*======================================================================================================================*/

