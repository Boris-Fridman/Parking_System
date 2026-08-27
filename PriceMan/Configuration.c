
/*======================================================================================================================*/

#include "Configuration.h"

#include <string.h>

#include <libgen.h> /* Required header for dirname() */

#include "ConfMan.h"


/*======================================================================================================================*/

#define CONFIGPATH     "CONFIGPATH"                                   /* Path to folder with configuration data.                                                                                                                         */
#define DATABASEPATH   "DATABASEPATH"                                 /* Path to database file.                                                                                                                                          */
#define PROGINFOPATH   "PROGINFOPATH"                                 /* Path to file containint the PID of the main process of this running program. Is used for priceman for sending the database update signals.                      */


#define NUM_CONF_IT   3                                               /* The number of items existing in the confituration ini file.                                                                                                     */
char ConfData[NUM_CONF_IT][2][PATH_LEN] =                             /* The Array of string-pairs containing the configuration items when the first string of pair is a key na dthe second one is the data.                             */
 {
  { CONFIGPATH      , "../" CONF_DIR_NAME                         },  /* Path to folder with configuration data.                                                                                                                         */
  { DATABASEPATH    , "../" CONF_DIR_NAME "/" DB_FILENAME         },  /* Path to database file.                                                                                                                                          */
  { PROGINFOPATH    , "../" CONF_DIR_NAME "/" DB_MAN_PID_FILENAME },  /* Path to file containint the PID of the main process of this running program. Is used for priceman for sending the database update signals.                      */
 };


/*======================================================================================================================*/

/* 
 * *************************************************************************************************************
 **          Initialization Configuration Procedure
 * *************************************************************************************************************
 */

/*----------------------------------------------------------------------------------------------------------------------*/
/* Initilizing fonfiguration procedure.                                                                                 */
void InitConfiguration(char const *OwnProgName)
 {
  InitConf(ConfData, NUM_CONF_IT, OwnProgName);  
 }

/*======================================================================================================================*/

/* 
 * *************************************************************************************************************
 **          Confituration returning parameters Functions
 * *************************************************************************************************************
 */

/*----------------------------------------------------------------------------------------------------------------------*/
/* Returns the path and name of the database file.                                                                      */
char const *GetDataBaseFilePathName()
 {
  return GetDataByName(ConfData, NUM_CONF_IT, DATABASEPATH);
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Returns the path with name of the file containing the file with the PID of this program.                             */
char const *GetProgInfoPIDFilePathName()  /* Path & Name of the file with the program PID. */
 {
  return GetDataByName(ConfData, NUM_CONF_IT, PROGINFOPATH);
 }







/*======================================================================================================================*/

