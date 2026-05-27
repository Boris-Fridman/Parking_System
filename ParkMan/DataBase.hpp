#pragma once

#include "cstdint"
#include "CommonData.h"

struct PriceTab_s
 {
  uint16_t City_ID;
  uint16_t Price;              /*  0.01₪ / hour  */
  char City_Name[NAME_LEN];
 };


struct CustomerTab_s
 {
  GPS_Cords_s Cords;
  uint32_t Customer_ID;
  uint16_t Fee;                /*  0.01₪  */
 };


 