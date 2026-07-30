#include "Configuration.h"

#include <string.h>

#include <libgen.h> // Required header for dirname()

#include "ConfMan.h"



#define CONFIGPATH     "CONFIGPATH"
#define DATABASEPATH   "DATABASEPATH"
#define PROGINFOPATH   "PROGINFOPATH"


#define NUM_CONF_IT   3
char ConfData[NUM_CONF_IT][2][PATH_LEN] = 
 {
  { CONFIGPATH      , "../" CONF_DIR_NAME                         },
  { DATABASEPATH    , "../" CONF_DIR_NAME "/" DB_FILENAME         },
  { PROGINFOPATH    , "../" CONF_DIR_NAME "/" DB_MAN_PID_FILENAME },
 };


void InitConfiguration(char const *OwnProgName)
 {
  InitConf(ConfData, NUM_CONF_IT, OwnProgName);  
 }


char const *GetDataBaseFilePathName()
 {
  return GetDataByName(ConfData, NUM_CONF_IT, DATABASEPATH);
 }

char const *GetProgInfoPIDFilePathName()  // Path & Name of the file with the program PID.
 {
  return GetDataByName(ConfData, NUM_CONF_IT, PROGINFOPATH);
 }









