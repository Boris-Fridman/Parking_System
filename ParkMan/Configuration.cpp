
/*======================================================================================================================*/

#include "Configuration.hpp"

#include <iostream>
#include <fstream>
#include <cstring>

#include <libgen.h> /* Required header for dirname() */
#include "ConfMan.h"


/*======================================================================================================================*/

#define CONFIGPATH     "CONFIGPATH"                                   /* Path to folder with configuration data.                                                                                                                         */
#define DATABASEPATH   "DATABASEPATH"                                 /* Path to database file.                                                                                                                                          */
#define SHAPEFILESPATH "SHAPEFILESPATH"                               /* Path to whape file containing polygons with GPS coordinates on corners bounding regions. Is used for detection geographic places according the GPS coordinates. */
#define PROGINFOPATH   "PROGINFOPATH"                                 /* Path to file containint the PID of the main process of this running program. Is used for priceman for sending the database update signals.                      */
#define LOGFILEPATH    "LOGFILEPATH"                                  /* Path to log file.                                                                                                                                               */
#define PORTNUMBER     "PORTNUMBER"                                   /* Port number that is opened during running the program for receiving information sent from clients.                                                              */
#define SHAPECITYNAME  "SHAPECITYNAME"                                /* The name of the field in the shape file of cities where is wirtten the the city name.                                                                           */
#define SHAPEOSMID     "SHAPEOSMID"                                   /* The name of the field in the shape file of cities where is written the region code according to the Open Street Map.                                            */


ConfData_s ConfData[] =                                               /* The Array of string-pairs containing the configuration items when the first string of pair is a key na dthe second one is the data.                             */
 {
  { CONFIGPATH      , "../" CONF_DIR_NAME                         },  /* Path to folder with configuration data.                                                                                                                         */
  { DATABASEPATH    , "../" CONF_DIR_NAME "/" DB_FILENAME         },  /* Path to database file.                                                                                                                                          */
  { SHAPEFILESPATH  , "../" CONF_DIR_NAME "/" SHP_FILENAME        },  /* Path to whape file containing polygons with GPS coordinates on corners bounding regions. Is used for detection geographic places according the GPS coordinates. */
  { PROGINFOPATH    , "../" CONF_DIR_NAME "/" DB_MAN_PID_FILENAME },  /* Path to file containint the PID of the main process of this running program. Is used for priceman for sending the database update signals.                      */
  { LOGFILEPATH     , LOG_FILENAME                                },  /* Path to log file.                                                                                                                                               */
  { PORTNUMBER      , TO_STRING(DESTIN_PORT)                      },  /* Port number that is opened during running the program for receiving information sent from clients.                                                              */
  { SHAPECITYNAME   , SHP_FIELD_NAME                              },  /* The name of the field in the shape file of cities where is wirtten the the city name.                                                                           */
  { SHAPEOSMID      , SHP_OSM_ID                                  },  /* The name of the field in the shape file of cities where is written the region code according to the Open Street Map.                                            */
 };
#define NUM_CONF_IT     (sizeof(ConfData) / sizeof(ConfData_s))       /* The number of items existing in the confituration ini file.                                                                                                     */


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
/* Returns the path and name of the file sith geographic places' regions.                                               */
char const *GetGeoLocShapeFilePathName()  /* GeoLocation Shape Path & Name of the shape file. */
 {
  return GetDataByName(ConfData, NUM_CONF_IT, SHAPEFILESPATH);
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Returns the path with name of the file containing the file with the PID of this program.                             */
char const *GetProgInfoPIDFilePathName()  /* Path & Name of the file with the program PID. */
 {
  return GetDataByName(ConfData, NUM_CONF_IT, PROGINFOPATH);
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Returns the path with name of the file containing the logging information.                                           */
char const *GetLogFilePathName()          /* Path & Name of the file with the program log. */
 {
  return GetDataByName(ConfData, NUM_CONF_IT, LOGFILEPATH);
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Returns the network port to which the clients must connect.                                                          */
uint16_t GetDestinPort()
 {
  int Result;
  char *endptr;
  char const *PortNumStr = GetDataByName(ConfData, NUM_CONF_IT, PORTNUMBER);
  Result = strtol(PortNumStr, &endptr, 10);
  if(endptr == PortNumStr)
   return DESTIN_PORT;
  else
   return Result;
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Returns the name of the field in the shape file with the name of the city, village or other geographic place.        */
char const *GetColumnNameWithGeolocationName()
 {
  return GetDataByName(ConfData, NUM_CONF_IT, SHAPECITYNAME);
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Returns the name of the field in the shape file with the Open Street Map code of the geographic place.               */
char const *GetColumnNameWithOSMID()
 {
  return GetDataByName(ConfData, NUM_CONF_IT, SHAPEOSMID);
 }

/*======================================================================================================================*/

