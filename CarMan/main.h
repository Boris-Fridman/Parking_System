#ifndef ____main_h__
#define ____main_h__

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
//#include <stdlib.h>


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
  // void set_flag(ProcTypeID_e flagno, bool state){exit_proc_flags = ((exit_proc_flags & (~(0x01<<flagno))) | (state<<flagno));};
  // bool get_flag(ProcTypeID_e flagno){return ((exit_proc_flags >> flagno) & 0x01);};
  // TskContShmData_s():exit_proc_flags(0){};
 }TskContShmData_s;




#endif    //  ____main_h__



