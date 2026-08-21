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



/**
 * @brief Generates a random number that is used for selecting parking place.
 *
 * @code
 * void GenRandParkNumber();
 * @code
 *
 * @return The generated parking number.
 */
void GenRandParkNumber();

/**
 * @brief Returns a parking places coordinates according a parking number.
 *
 * @code
 * void GetRandParking(int *ParkingInd, ParkingData_s *Parking);
 * @code
 *
 * @param ParkingInd   Returned integer number contains a parking index that is randomly selected.
 *
 * @param Parking      Returned structure containing the parking place's data: Parking Name and GPS coordinates.
 */
void GetRandParking(int *ParkingInd, ParkingData_s *Parking);



#endif /* PARKINGSDATA_H_ */
