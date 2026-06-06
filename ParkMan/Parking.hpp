#pragma once
#include "CommonData.h"

#include <sys/types.h>

class Customer_c
 {
  public:
    Customer_c();
 };

class City_c
 {
    GPS_Cords_s CenterPlace;
  public:
    City_c();
    virtual ~City_c();
    virtual bool Customer_In() = 0;
    
 };


void ParkingProc(key_t sh_mem_key, const char sem_name[]);



