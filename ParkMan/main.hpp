#pragma once
#include <cstdint>
#include <sys/types.h>
#include <string>
#include "CommonData.h"


#define MAIN_PROC_NAME     (char *)"Main    "     /* Main process name*/

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

struct ControlDBPrice_s
 { 
  key_t CitiesNewShmKey = 0;
  char CitiesSemName[NAME_LEN] = {""};
  char ReportQueueName[NAME_LEN] = {""};
  char ReportSemName[NAME_LEN] = {""};
  uint16_t NumPriceDBCities = 0;
  bool DBUpdateRequired = false; 
  //bool DBUpdated = false;
  std::string DBFileName = "";  /* File name including path to database with parking information. */
  std::string SHPFileName = ""; /* File name including path to shapefile containing inforamtion aout border of the cities. */
 };


struct TskContShmData_s
 {
  ControlDBPrice_s ControlDBPriceShMem = {0};
  uint8_t exit_proc_flags;
  void set_flag(ProcTypeID_e flagno, bool state){exit_proc_flags = ((exit_proc_flags & (~(0x01<<flagno))) | (state<<flagno));};
  bool get_flag(ProcTypeID_e flagno){return ((exit_proc_flags >> flagno) & 0x01);};
  TskContShmData_s():exit_proc_flags(0){};
 };
