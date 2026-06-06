#include "Parking.hpp"
#include "main.hpp"
#include "Errors.hpp"
#include "Processes.hpp"

#include <semaphore.h>
#include <unistd.h>
#include <iostream>
#include <sys/sem.h>
#include <sys/shm.h>


#define PARK_PROC_NAME     (char *)"Parking"     /* Parking process name*/


Customer_c::Customer_c()
 {

 }


City_c::City_c()
  :CenterPlace({0,0})
 {

 }

City_c::~City_c()
 {

 }



void ParkingProc(key_t sh_mem_key, const char sem_name[])
 {
  ProcParam_s MainParams = PROC_INIT_VAL;
  OnStartProcess(&MainParams , PARK_PROC_NAME, sh_mem_key, sem_name);


  do
   {
    sleep(1);
    MainParams.exit_required |= MainParams.p_shm->exit_parking;     
    MainParams.exit_required |= (getppid() == 1); // Checking if the parent process is running. If not enables exit.
   } 
  while (!MainParams.exit_required);


  OnEndProcess(&MainParams , PARK_PROC_NAME);  

 }

