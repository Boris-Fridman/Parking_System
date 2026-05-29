#ifndef ____DataBase_h__
#define ____DataBase_h__


/*======================================================================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "CommonData.h"
#include <sqlite3.h>

/*======================================================================================================================*/

/**
 * @brief Loads existing database or creates a new one if any database doesn't exist.
 * 
 * @code
 * int CreateLoadDatabase(sqlite3 **conn);
 * @code
 * 
 * @param conn pointer to the sqlite database handle
 * 
 * @return "0" if success, "-1" if the database cannot be created or -2 if the pointer is problematic or not given.
 */
int CreateLoadDatabase(sqlite3 **conn);


/** 
 * @brief Checks all the cities' ids in the database and returns the biggest one. (It's required to decide which test id must be given to the new city.)
 * 
 * @code
 * int GetLastCityIDFromDataBase(sqlite3 **conn);
 * @code
 * 
 * @param conn pointer to the sqlite database handle.
 * 
 * @return The last city id.
 */
int GetLastCityIDFromDataBase(sqlite3 **conn);


int FindCityInDataBase(sqlite3 **conn, const char name[]);

int UpdateCityInDataBase(sqlite3 **conn, char city_name[], int city_price);

/** 
 * @brief Writes new city to the database.
 * 
 * @code
 * int WriteToDataBase(sqlite3 **conn, int test_id, char date_time[], TestResType_e result);
 * @code
 * 
 * @param conn pointer to the sqlite database handle.
 * 
 * @param city_id the id of the city to be added. 
 * 
 * @param city_name the name of the city to be added. 
 * 
 * @param city_price the price of the parking in ₪ per hour.
 * 
 * @return One of the database CAPI3REF Result Codes. (See lines 434 - 476 in the "sqlite3.h"-file).
 */
int WriteToDataBase(sqlite3 **conn, int city_id, char city_name[], int city_price);


int RemoveCityFromDataBase(sqlite3 **conn, char city_name[]);


/*======================================================================================================================*/


#endif //  ____DataBase_h__


