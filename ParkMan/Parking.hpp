#pragma once

/*======================================================================================================================*/

#include "CommonData.h"
#include "main.hpp"

#include "DataBase.hpp"

#include <sys/types.h>


#define PARK_PROC_NAME     (char *)"Parking"     /* Parking process name*/



// The "ShapeFileName" should be got by the function "TaskControl_ShSM_c::GetSHPFileName()".
bool DetectCity(GPS_Cords_s Cords, std::string &CityName, uint32_t &RegionCode, uint32_t &EdRegCode,  std::string &ShapeFileName);

bool StringsAreEqual(std::string str1, std::string str2);
void RemoveUnneededChars(std::string &StringToClear);
void ReplaceSubStrings(std::string &StringToCorrect);


void HandleClient(int ClientSocket, uint16_t NumPriceDBCities = 0, DBShmemPriceData_c **DBShmemPriceData = nullptr, std::string ShapeFileName = "", Process_c *NetCl = nullptr, std::string AddrStamp = "        ---        "); 


/*======================================================================================================================*/
