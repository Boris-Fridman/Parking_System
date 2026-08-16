#ifndef ____Configuration_h__
#define ____Configuration_h__

#include "CommonData.h"

/**
 * Attention !!!
 * This prucudure must be run from the main file before creating any external process to ensure that the copied data to a new-created-process is allready up-to-date.
 * 
*/

void InitConfiguration(char const *OwnProgName);

char const *GetLogFilePathName();          // Path & Name of the file with the program log.

bool GetUseDHCPState();

char const *GetDestinDHCPName();

char const *GetDestinAddr();

uint16_t GetDestinPort();

char const *GetClientName();

uint32_t GetVechicleID();

uint32_t GetMaxParkTime();

uint32_t GetMaxParkWaitTime();














#endif  //  ____Configuration_h__

