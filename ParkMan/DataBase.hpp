#pragma once

#include "cstdint"
#include "CommonData.h"


struct CustomerTab_s
 {
  GPS_Cords_s Cords;
  uint32_t Customer_ID;
  uint16_t Fee;                /*  0.01₪  */
 };


 