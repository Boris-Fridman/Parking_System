#pragma once

#define SH_MEM_SIZE     (sizeof(ShmData_s))


typedef struct ShmData_s
 {
  bool exit_database;
  bool exit_network;
  bool exit_parking;
  bool exit_existerrors;
 }
ShmData_s;
