#ifndef ____Configuration_h__
#define ____Configuration_h__

/*======================================================================================================================*/

#include "CommonData.h"

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
 * @brief Returns the DHCP Name of the server.
 * 
 * @code
 * bool GetUseDHCPState();
 * @code
 * 
 * @return "0" - Static IP Address is set in .ini file or "1" - DHCP Server Name is set in the .ini file.
 */
bool GetUseDHCPState();

/**
 * @brief Returns the DHCP Name of the server.
 * 
 * @code
 * char const *GetDestinDHCPName();
 * @code
 * 
 * @return DHCP Server Name written in the .ini file.
 */
char const *GetDestinDHCPName();

/**
 * @brief Returns the static IP Address of the server.
 * 
 * @code
 * char const *GetDestinAddr();
 * @code
 * 
 * @return Static Server Address written in the .ini file.
 */
char const *GetDestinAddr();

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
 * @brief Returns the Name of the client for parking information.
 * 
 * @code
 * char const *GetClientName();
 * @code
 * 
 * @return Name of the Customer sent in parking data via network, written in the .ini file
 */
char const *GetClientName();

/**
 * @brief Returns the Vehicle ID of the client for parking information.
 * 
 * @code
 * uint32_t GetVechicleID();
 * @code
 * 
 * @return Vehicle ID of the Customer sent in parking data via network, written in the .ini file
 */
uint32_t GetVechicleID();

/**
 * @brief Returns the maximal parking time that can be randomely generated or "0" if it is infinit.
 * 
 * @code
 * uint32_t GetMaxParkTime();
 * @code
 * 
 * @return Maximal parking time in seconds written in the .ini file.
 */
uint32_t GetMaxParkTime();

/**
 * @brief Returns the maximal pause time between the parkings that can be randomely generated.
 * 
 * @code
 * uint32_t GetMaxParkWaitTime();
 * @code
 * 
 * @return Maximal pause time that can be generated between parkings.
 */
uint32_t GetMaxParkWaitTime();














#endif  //  ____Configuration_h__

