#pragma once

/*======================================================================================================================*/

#include "CommonData.h"
#include "main.hpp"


#include <sys/types.h>


#define PARK_PROC_NAME     (char *)"Parking "     /* Parking process name*/



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


void ParkingProc(key_t sh_mem_key, const char sem_name[], std::string sq_name, std::string qsem_name, ProcTypeID_e ProcType);

// The "ShapeFileName" should be got by the function "TaskControl_ShSM_c::GetSHPFileName()".
bool DetectCity(GPS_Cords_s Cords, std::string &CityName, uint32_t &RegionCode, uint32_t &EdRegCode,  std::string &ShapeFileName);

bool StringsAreEqual(std::string str1, std::string str2);
void RemoveUnneededChars(std::string &StringToClear);
void ReplaceSubStrings(std::string &StringToCorrect);

/*======================================================================================================================*/
