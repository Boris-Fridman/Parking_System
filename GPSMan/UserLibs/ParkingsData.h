/*
 * ParkingsData.h
 *
 *  Created on: 8 Jul 2026
 *      Author: boris
 */

#ifndef PARKINGSDATA_H_
#define PARKINGSDATA_H_

#include "main.h"
#include "CommonData.h"

typedef struct
ParkingData_s
 {
  char ParkingName[64];
  GPS_Cords_s ParkingCords;
 }
ParkingData_s;



extern ParkingData_s const ParkingData[];
extern const size_t PARKDATA_SIZE;

#endif /* PARKINGSDATA_H_ */
