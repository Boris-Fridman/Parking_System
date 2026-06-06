#pragma once

#include "CommonData.h"
#include "main.hpp"

#include <sys/types.h>


void NetworkProc(key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType);

