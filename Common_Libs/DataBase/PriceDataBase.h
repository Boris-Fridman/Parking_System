#ifndef ____PriceDataBase_h__
#define ____PriceDataBase_h__

/*======================================================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*======================================================================================================================*/
#ifdef __cplusplus
#include <cstdint>
#include <cstdbool>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

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
 * @brief Checks all the cities' ids in the database and returns the not esisting one. (Tries to find a smallest not existing.)
 *        If the smallest not existing ID was found it is retunrd otherwise is teturnd the one above the biggest existing.
 * 
 * @code
 * int GetCityIDNotExistingInDataBase(sqlite3 **conn);
 * @code
 * 
 * @param conn pointer to the sqlite database handle.
 * 
 * @return The last city id.
 */
int GetCityIDNotExistingInDataBase(sqlite3 **conn);

/**
 * @brief Finds city by name in the database.
 * 
 * @code
 * int FindCityInDataBase(sqlite3 **conn, const char name[]);
 * @code
 * 
 * @param conn pointer to the sqlite database handle.
 * 
 * @param city_name the name of the city to find. 
 * 
 * @return the first row number where the city was found, "-1" if error or "-3" if wasn't found.
 */
int FindCityInDataBase(sqlite3 **conn, const char city_name[]);

/**
 * @brief Finds parking session by vehicle id, client name and start parking time in the database.
 * 
 * @code
 * int FindParkSessionInDataBase(sqlite3 **conn, uint32_t vehicle_id, const char client_name[], time_t parking_start_time);
 * @code
 * 
 * @param conn pointer to the sqlite database handle.
 * 
 * @param vehicle_id the ID of the parking vehicle.
 * 
 * @param client_name the name of the city to find.
 * 
 * @param parking_start_time start time of the parking. - Is required to detemine the parking session.
 * 
 * @return the first row number where the client was found, "-1" if error or "-3" if wasn't found.
 */
int FindParkSessionInDataBase(sqlite3 **conn, uint32_t vehicle_id, const char client_name[], time_t parking_start_time);


/**
 * @brief Updates existing city in the database.
 * 
 * @code
 * int UpdateCityPriceInDataBase(sqlite3 **conn, char city_name[], int city_price);
 * @code
 * 
 * @param conn pointer to the sqlite database handle.
 * 
 * @param city_name the name of the city to be updated. 
 * 
 * @param city_price the new price of the parking in ₪ per hour.
 * 
 * @return "0" if success, "-1" if error or "-3" if the city wasn't found.
 */
int UpdateCityPriceInDataBase(sqlite3 **conn, char const city_name[], int const city_price);

/**
 * @brief Updates existing parking session in the database.
 * 
 * @code
 * int UpdateParkSessionInDataBase(sqlite3 **conn, ClientQueueMsg_s client_queue_msg);
 * @code
 * 
 * @param conn pointer to the sqlite database handle.
 * 
 * @param client_queue_msg the structure containint the parking session data.
 * 
 * @return "0" if success, "-1" if error or "-3" if the city wasn't found.
 */
int UpdateParkSessionInDataBase(sqlite3 **conn, ClientQueueMsg_s client_queue_msg);


/** 
 * @brief Writes new city to the database.
 * 
 * @code
 * int WriteNewCityToDataBase(sqlite3 **conn, int test_id, char date_time[], TestResType_e result);
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
int WriteNewCityToDataBase(sqlite3 **conn, int city_id, char const city_name[], int const city_price);


/** 
 * @brief Writes new parking session to the database.
 * 
 * @code
 * int WriteNewParkSessionToDataBase(sqlite3 **conn, ClientQueueMsg_s ClientQueueMsg);
 * @code
 * 
 * @param conn pointer to the sqlite database handle.
 * 
 * @param client_queue_msg the structure containint the parking session data.
 * 
 * @return One of the database CAPI3REF Result Codes. (See lines 434 - 476 in the "sqlite3.h"-file).
 */
int WriteNewParkSessionToDataBase(sqlite3 **conn, ClientQueueMsg_s client_queue_msg);


/**
 * @brief Removes the given city from the database.
 * 
 * @code
 * int RemoveCityFromDataBase(sqlite3 **conn, char city_name[]);
 * @code
 * 
 * @param conn pointer to the sqlite database handle.
 * 
 * @param city_name name of the city to remove.
 * 
 * @return "0" if success, "-1" if error or "-3" if the city wasn't found.
 */
int RemoveCityFromDataBase(sqlite3 **conn, char city_name[]);


/**
 * @param Renames city according old and new names.
 * 
 * @code
 * int RenameCityByName(sqlite3 **conn, char old_name[], char new_name[]);
 * @code
 * 
 * @param conn pointer to the sqlite database handle.
 * 
 * @param old_name name of the city to rename.
 * 
 * @param new_name the new name to be given to the city.
 * 
 * @return "0" success, "-1" couldn't be renamed, "-3" the city wasn't found.
 */
int RenameCityByName(sqlite3 **conn, char old_name[], char new_name[]);

/**
 * @brief Gives a list of the all existing cities in the database.
 * 
 * @code 
 * int GetCitiesList(sqlite3 **conn, PriceTab_s **list, int *list_size);
 * @code
 * 
 * @param conn pointer to the sqlite database handle.
 * 
 * @param list the pointer to pointer to list. If the memory allocation failed the pointer will be NULL.
 *             Attention!!!
 *             The function reserves internal dinamic menory 
 *             that must be freed by the function "FreeList()" after usage.
 * 
 * @param list_size pointer to the list size that after creation. 
 *                  if memory wasn't reserved or the database is empty the function will give it zero value.
 * 
 * @return "0" if successfully, "-1" if the database couldn't be open, "-2" if the memory couldn't be reserved or "-3" if the list couldn't be filled.
 * 
 */
int GetCitiesList(sqlite3 **conn, PriceTab_s **list, int *list_size);

/**
 * @brief Frees list after usage reserved by the "GetCitiesList()" function.
 *        No need to check if the list is NULL because the function checks it inside.
 *        Even more: This function should be run anyway to prevent possible dynamic memory leakage.
 * 
 * @code
 * void FreeList(PriceTab_s **list_to_free);
 * @code
 * 
 * @param list_to_free list to be freed reserved by the function "GetCitiesList()".
 * 
 */
void FreeList(PriceTab_s **list_to_free);

/**
 * @brief Sets DataBase file name exactly as in parameter. (Was defined for multiprocess variant.)
 * 
 * @code
 * void SetDBPathName(char FileNameToSet[]);
 * @code
 * 
 * @param FileNameToSet The name that must be set.
 * 
 */
void SetDBPathName(char const FileNameToSet[]);


/**
 * @brief Applys the path to the database file.
 * 
 * @code 
 * void ApplyDBPath(int const argc, char const *argv[]);
 * @code
 * 
 * @param argc  The argc taken from the main() parameter: the count of the aguments.
 * 
 * @param argv  The argv taken from the main() parameter: the vector of the arguments.
 * 
 */
void ApplyDBPath(int const argc, char const *argv[]);

/*======================================================================================================================*/

#ifdef __cplusplus
}
#endif

/*======================================================================================================================*/

#endif //  ____DataBase_h__


