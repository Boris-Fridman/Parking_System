#pragma once
#include "CommonData.h"

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
    virtual bool Customer_In() = 0;
     
 };


