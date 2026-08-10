#pragma once

#include "CommonData.h"
#include "main.hpp"

#include <sys/types.h>


#define NETW_PROC_NAME     (char *)"Network "     /* Network process name*/


void NetworkProc(key_t sh_mem_key, const char sem_name[], std::string sq_name, std::string qsem_name, ProcTypeID_e ProcType);

