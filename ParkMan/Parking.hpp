#pragma once

/*======================================================================================================================*/

#include "CommonData.h"
#include "main.hpp"

#include "DataBase.hpp"

#include <sys/types.h>


/*======================================================================================================================*/


#define PARK_PROC_NAME     (char *)"Parking"     /* Parking process name*/

/*======================================================================================================================*/


/**
 * @brief The session parking task procedure. Runs untill the client is connected and calculates praking price.
 * 
 * @code
 * void HandleClient(int ClientSocket, uint16_t NumPriceDBCities = 0, DBShmemPriceData_c **DBShmemPriceData = nullptr, std::string ShapeFileName = "", Process_c *NetCl = nullptr, std::string AddrStamp = "        ---        "); 
 * @code
 * 
 * @param ClientSocket     The socket through which the cliend is connected.
 * 
 * @param NumPriceDBCities The number of the citiew in the DataBase.
 * 
 * @param DBShmemPriceData The information of the database shared memory where is stored the cities' price table.
 * 
 * @param ShapeFileName    The name of the shape file containing the cities information that is used for dectecting the parking place according to the given GPS coordinates.
 * 
 * @param NetCl            The pointer to the Netrowk object running the main process. Is used for detecting the main parent process' properties. Instead of the pointer to the Network process there can be given the pointer to the parent process from which the network process is derived.
 * 
 * @param AddrStamp        The network address given as a string.
 */
void HandleClient(int ClientSocket, uint16_t NumPriceDBCities = 0, DBShmemPriceData_c **DBShmemPriceData = nullptr, std::string ShapeFileName = "", Process_c *NetCl = nullptr, std::string AddrStamp = "___.___.___.___:___"); 


/*======================================================================================================================*/
