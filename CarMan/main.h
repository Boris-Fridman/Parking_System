#ifndef ____main_h__
#define ____main_h__

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "CommonData.h"
//#include <stdlib.h>


#define MAIN_PROC_NAME     (char *)"Main    "     /* Main process name*/


typedef enum ProcTypeID_e
 {
  PROC_I2C_E,
  PROC_NETWORK_E,
  /* ... */
  PROC_NUM_PROC_TYPES_E
 }ProcTypeID_e;


typedef struct TskContShmData_s
 {
  uint8_t exit_proc_flags;
  char NetQueueName[NAME_LEN]; /* Is used for communication between Network and I2C Processes. */
 }TskContShmData_s;




#endif    //  ____main_h__



