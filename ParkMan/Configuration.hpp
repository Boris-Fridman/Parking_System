#pragma once

/*======================================================================================================================*/

#include "main.hpp"

/**
 * Attention !!!
 * The prucudure "InitConfiguration()" must be run from the main file before creating any external process to ensure that the copied data to a new-created-process is allready up-to-date.
 * 
*/

/*======================================================================================================================*/

/**
 * @brief Initilizing fonfiguration procedure.
 *        Attention !!!
 *        This prucudure must be run from the main file before creating any external process to ensure that the copied data to a new-created-process is allready up-to-date.
 * 
 * @code
 * void InitConfiguration(char const *OwnProgName);
 * @code
 * 
 * @param OwnProgName The name with path of the current running program file.
 */
void InitConfiguration(char const *OwnProgName);

/*======================================================================================================================*/

/**
 * @brief Returns the path and name of the database file.
 * 
 * @code
 * char const *GetDataBaseFilePathName();
 * @code
 * 
 * @return THe path and name of the database file.
 */
char const *GetDataBaseFilePathName();

/**
 * @brief Returns the path and name of the file sith geographic places' regions.
 * 
 * @code
 * char const *GetGeoLocShapeFilePathName();
 * @code
 * 
 * @return GeoLocation Shape Path & Name of the shape file.
 */
char const *GetGeoLocShapeFilePathName();  /* GeoLocation Shape Path & Name of the shape file. */

/**
 * @brief Returns the path with name of the file containing the file with the PID of this program.
 * 
 * @code
 * char const *GetProgInfoPIDFilePathName();
 * @code
 * 
 * @return Path & Name of the file with the program PID.
 */
char const *GetProgInfoPIDFilePathName();  /* Path & Name of the file with the program PID. */

/**
 * @brief Returns the path with name of the file containing the logging information.
 * 
 * @code
 * char const *GetLogFilePathName();
 * @code
 * 
 * @return Path & Name of the file with the program log.
 */
char const *GetLogFilePathName();          /* Path & Name of the file with the program log. */

/**
 * @brief Returns the network port to which the clients must connect.
 * 
 * @code
 * uint16_t GetDestinPort();
 * @code
 * 
 * @return The port in the Network to which the clients' connection.
 */
uint16_t GetDestinPort();

/**
 * @brief Returns the name of the field in the shape file with the name of the city, village or other geographic place.
 * 
 * @code
 * char const *GetColumnNameWithGeolocationName();
 * @code
 * 
 * @return The name of the field in the shape file of cities where is wirtten the the city name.
 */
char const *GetColumnNameWithGeolocationName();

/**
 * @brief Returns the name of the field in the shape file with the Open Street Map code of the geographic place.
 * 
 * @code
 * char const *GetColumnNameWithOSMID();
 * @code
 * 
 * @return The name of the field in the shape file of cities where is written the region code according to the Open Street Map.
 */
char const *GetColumnNameWithOSMID();

/*======================================================================================================================*/
