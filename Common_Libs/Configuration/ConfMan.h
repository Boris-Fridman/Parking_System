#ifndef ____ConfMan_h__
#define ____ConfMan_h__

/*======================================================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*======================================================================================================================*/

#include "CommonData.h"

/*======================================================================================================================*/

#define KEY_LEN  NAME_LEN    /* Length of the key in vocobulary pair.   */
#define VAL_LEN  PATH_LEN    /* Length of the value in vocobulary pair. */

/*======================================================================================================================*/

/*----------------------------------------------------------------------------------------------------------------------*/
/* Defines vocobulary of the configuration items.                                                                       */
#ifndef __cplusplus
typedef 
#endif
struct ConfData_s
 {
  char Key[KEY_LEN];    /* The name of configuration option.               */
  char Value[VAL_LEN];  /* The value existing in the configuration option. */
 }
 #ifndef __cplusplus
 ConfData_s
 #endif
 ;


/*======================================================================================================================*/

/**
 * @brief Initilizes configuration.
 * 
 * @code
 void InitConf(ConfData_s ConfData[], size_t const NumConfIt, char const *OwnProgName);
 * @code
 * 
 * @param ConfData    Congiguration data built as vocobulary of strings' pairs 
 *                    when the first string is key and the second one is the data.
 * 
 * @param NumConfIt   Number of items (strings' pairs) existing in the configuration.
 * 
 * @param OwnProgName The name of the current program including path for determining .ini file.
 */
void InitConf(ConfData_s ConfData[], size_t const NumConfIt, char const *OwnProgName);

/**
 * @brief Gives data by a given name.
 * 
 * @code
 char const *GetDataByName(ConfData_s ConfData[], size_t const NumConfIt, char const DataName[]);
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
char const *GetDataByName(ConfData_s ConfData[], size_t const NumConfIt, char const DataName[]);


/*======================================================================================================================*/

#ifdef __cplusplus
}
#endif

/*======================================================================================================================*/

#endif  /*  ____ConfMan_h__  */


