#ifndef ____Configuration_h__
#define ____Configuration_h__

#include "CommonData.h"

/**
 * Attention !!!
 * This prucudure must be run from the main file before creating any external process to ensure that the copied data to a new-created-process is allready up-to-date.
 * 
*/

void InitConfiguration(char const *OwnProgName);

char const *GetDataBaseFilePathName();

char const *GetProgInfoPIDFilePathName();  // Path & Name of the file with the program PID.







#endif  //  ____Configuration_h__

