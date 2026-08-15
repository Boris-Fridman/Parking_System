#ifndef ____I2C_h__
#define ____I2C_h__

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "Processes.h"


#define I2C_PROC_NAME     (char *)"I2C     "     /* I2C process name*/



void I2CProc(ProcParams_s *ProcParams);


#endif  //  ____I2C_h__


