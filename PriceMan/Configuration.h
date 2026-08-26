#ifndef ____Configuration_h__
#define ____Configuration_h__

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
 * @brief Returns the path with name of the file containing the file with the PID of this program.
 * 
 * @code
 * char const *GetProgInfoPIDFilePathName();
 * @code
 * 
 * @return Path & Name of the file with the program PID.
 */
char const *GetProgInfoPIDFilePathName();  // Path & Name of the file with the program PID.







#endif  //  ____Configuration_h__

