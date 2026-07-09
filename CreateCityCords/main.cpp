#include <iostream>
#include "CommonData.h"
#include "CitiesInfo.hpp"
#include "cmath"

#include "CitiesCords.hpp"

#define NUM_TOWNS 4

GPS_Cords_s p0 = { 1, 1},
            p1 = {-1,-1}, 
            p2 = { 1,-1}, 
            p3 = {-1, 1},

            pt = { 0, 0};

CityData_s CitiesList[NUM_TOWNS] = 
 {
  {"Tel Aviv",      {{ 0, 0}, { 0, 2}, { 2, 2}, { 2, 0}}},
  {"Ramat Gan",     {{ 0, 0}, { 0,-2}, {-2,-2}, {-2, 0}}},
  {"Givataiim",     {{ 0, 0}, { 0,-2}, { 2,-2}, { 2, 0}}},
  {"Rishon Lezion", {{ 0, 0}, { 0, 2}, {-2, 2}, {-2, 0}}}
 };

CityData_s ShapeForTest = 
  {
   "Test Shape",
   {
/*  1 */    {  8,  8},
/*  2 */    {  8,  9},
/*  3 */    {  9,  9},
/*  4 */    { 12, 12},
/*  5 */    { 13, 12},
/*  6 */    { 13,  6},
/*  7 */    { 10,  3},
/*  8 */    { 10, -5},
/*  9 */    {  9, -6},
/* 10 */    {  7, -7},
/* 11 */    {  3, -7},
/* 12 */    {  1, -6},
/* 13 */    { -4, -1},
/* 14 */    { -5,  1},
/* 15 */    { -3,  7},
/* 16 */    { -3, 11},
/* 17 */    {  1, 15},
/* 18 */    {  6, 15},
/* 19 */    {  7, 14},
/* 20 */    {  7,  7},
/* 21 */    {  5,  7},
/* 22 */    {  5, 10},
/* 23 */    {  4, 11},
/* 24 */    {  4, 13},
/* 25 */    {  3, 13},
/* 26 */    { -1,  9},
/* 27 */    { -1,  7},
/* 28 */    {  3,  1},
/* 29 */    {  6,  1},
/* 30 */    { 11,  6},
/* 31 */    { 11,  7},
/* 32 */    { 10,  8}
   }
  };

int main()
 {
  
  PointState_e result; 
  std::string CityName;
  ssize_t CityNumber;
  IntArr_t BoundingCitiesList;
  ssize_t i;
  // result = PointInPoly(CitiesList[0].CityCords, pt);
  // switch(result)
  //  {
  //   case OUTSIZE_E:
  //     printf("outisde\n\r");
  //    break;
  //   case ONBOUND_E:
  //     printf("onbund\n\r");
  //    break;
  //   case INSIDE_E:
  //     printf("inside\n\r");
  //    break;
  //  }

  // GPS_Cords_s ShapeTestPoint = { 11, 3};
  // result = PointInPoly(ShapeForTest.CityCords, ShapeTestPoint);

  GPS_Cords_s TelAvivPoint = {34.78475, 32.09370};
  result = PointInPoly(Tel_Aviv_Shape.CityCords, TelAvivPoint);

  switch(result)
   {
    case OUTSIZE_E:
      printf("outisde\n\r");
     break;
    case ONBOUND_E:
      printf("onbund\n\r");
     break;
    case INSIDE_E:
      printf("inside\n\r");
     break;
   }
  printf("\n\r\n\r");



  DetectCityFromGPSCords(CitiesList, NUM_TOWNS, pt, CityName, CityNumber, BoundingCitiesList);

  if(CityNumber >= 0)
   {
    std::cout << "The point exists in the " << CityName << "\n\r";
   }
  else 
   if(BoundingCitiesList.size() > 0)
    {
     std::cout << "The Bounding cities:\n\r";
     for(i = 0; (size_t)i < BoundingCitiesList.size(); i++)
      {
       std::cout << CitiesList[BoundingCitiesList[i]].Name << "\n\r";
      }
    }
   else
    {
     std::cout << "No city was detected.\n\r";
    }



//   printf("%f\n\r" ,atan2( 1, 1)/M_PI);
//   printf("%f\n\r" ,atan2( 1,-1)/M_PI);
//   printf("%f\n\r" ,atan2(-1,-1)/M_PI);
//   printf("%f\n\r" ,atan2(-1, 1)/M_PI);
//   printf("%f\n\r" ,Atan2( 1, 1)/M_PI);
//   printf("%f\n\r" ,Atan2( 1,-1)/M_PI);
//   printf("%f\n\r" ,Atan2(-1,-1)/M_PI);
//   printf("%f\n\r" ,Atan2(-1, 1)/M_PI);
  //round(1.5);
 }




 