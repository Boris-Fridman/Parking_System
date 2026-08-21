#pragma once
#include <vector>
#include <string>
#include "CommonData.h"


#define PERMITED_ERROR 0.01

typedef std::vector<GPS_Cords_s> GPS_Poly_t;
typedef std::vector<ssize_t> IntArr_t;

struct CityData_s
 {
  std::string Name;
  GPS_Poly_t CityCords;
 };


double Atan2(double y, double x);
PointState_e PointInPoly(GPS_Poly_t BoundingPoly, GPS_Cords_s Cords); 
void DetectCityFromGPSCords(CityData_s const CitiesList[], ssize_t const NumSities, GPS_Cords_s Cords, std::string &CityName, ssize_t &Number, IntArr_t &BoundingCities);

