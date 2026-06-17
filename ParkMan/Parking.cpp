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

class Parking_c: public Process_c
 {
  public:
    Parking_c(char ProcName[], key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType);
    virtual ~Parking_c();
    virtual void OnRunProcess();
 };


void ParkingProc(key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType)
 {
  Parking_c Park_Process(PARK_PROC_NAME, sh_mem_key, sem_name, ProcType);
  Park_Process.RunProcess();
 }


void Parking_c::OnRunProcess()
 {
  Process_c::OnRunProcess();
 };



Parking_c::Parking_c(char ProcName[], key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType)
 :Process_c(ProcName, sh_mem_key, sem_name, ProcType)
 {

 }

Parking_c::~Parking_c()
 {

 }

