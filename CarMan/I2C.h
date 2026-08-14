#ifndef ____I2C_h__
#define ____I2C_h__

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "Processes.h"



void I2CProc(SlaveShMem_s *TskContShms, SlaveShQue_s *TskContShqs);


#endif  //  ____I2C_h__


