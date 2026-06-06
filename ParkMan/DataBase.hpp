#pragma once

#include "cstdint"
#include "main.hpp"

#include "CommonData.h"


#include <sys/types.h>

struct CustomerTab_s
 {
  GPS_Cords_s Cords;
  uint32_t Customer_ID;
  uint16_t Fee;                /*  0.01₪  */
 };


 void DataBaseProc(key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType);


 