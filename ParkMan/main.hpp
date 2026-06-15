#pragma once
#include <cstdint>

#define TSK_CONT_SH_MEM_SIZE     (sizeof(TskContShmData_s))


enum ProcTypeID_e
 {
  PROC_DATABASE_E,
  PROC_NETWORK_E,
  PROC_PARKING_E,
  PROC_ERRORS_E,
  /* ... */
  PROC_NUM_PROC_TYPES_E
 };



struct TskContShmData_s
 {
  uint8_t exit_proc_flags;
  void set_flag(ProcTypeID_e flagno, bool state){exit_proc_flags = ((exit_proc_flags & (~(0x01<<flagno))) | (state<<flagno));};
  bool get_flag(ProcTypeID_e flagno){return ((exit_proc_flags >> flagno) & 0x01);};
  TskContShmData_s():exit_proc_flags(0){};
 };
