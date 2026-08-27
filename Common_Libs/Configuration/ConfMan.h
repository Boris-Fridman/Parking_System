#ifndef ____ConfMan_h__
#define ____ConfMan_h__

/*======================================================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*======================================================================================================================*/

#include "CommonData.h"

/*======================================================================================================================*/

/**
 * @brief Initilizes configuration.
 * 
 * @code
 * void InitConf(char ConfData[][2][PATH_LEN], size_t NumConfIt, char const *OwnProgName);
 * @code
 * 
 * @param ConfData    Congiguration data built as vocobulary of strings' pairs 
 *                    when the first string is key and the second one is the data.
 * 
 * @param NumConfIt   Number of items (strings' pairs) existing in the configuration.
 * 
 * @param OwnProgName The name of the current program including path for determining .ini file.
 */
void InitConf(char ConfData[][2][PATH_LEN], size_t const NumConfIt, char const *OwnProgName);

/**
 * @brief Gives data by a given name.
 * 
 * @code
 * char const *GetDataByName(char ConfData[][2][PATH_LEN], size_t const NumConfIt, char const DataName[]);
 * @code
 * 
 * @param ConfData    Congiguration data built as vocobulary of strings' pairs 
 *                    when the first string is key and the second one is the data.
 * 
 * @param NumConfIt   Number of items (strings' pairs) existing in the configuration.
 * 
 * @param DataName    The key according which the data is given.
 * 
 * @return The pointer to start of data string that is given as constant to prevent modification.
 */
char const *GetDataByName(char ConfData[][2][PATH_LEN], size_t const NumConfIt, char const DataName[]);


/*======================================================================================================================*/

#ifdef __cplusplus
}
#endif

/*======================================================================================================================*/

#endif  /*  ____ConfMan_h__  */


