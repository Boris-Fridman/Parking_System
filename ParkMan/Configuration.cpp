#include "Configuration.hpp"

#include <iostream>
#include <fstream>
#include <cstring>

#include <libgen.h> // Required header for dirname()
#include "ConfMan.h"



#define CONFIGPATH     "CONFIGPATH"
#define DATABASEPATH   "DATABASEPATH"
#define SHAPEFILESPATH "SHAPEFILESPATH"
#define PROGINFOPATH   "PROGINFOPATH"
#define PORTNUMBER     "PORTNUMBER"
#define SHAPECITYNAME  "SHAPECITYNAME"
#define SHAPEOSMID     "SHAPEOSMID"     


#define NUM_CONF_IT   7
char ConfData[NUM_CONF_IT][2][PATH_LEN] = 
 {
  { CONFIGPATH      , "../" CONF_DIR_NAME                         },
  { DATABASEPATH    , "../" CONF_DIR_NAME "/" DB_FILENAME         },
  { SHAPEFILESPATH  , "../" CONF_DIR_NAME "/" SHP_FILENAME        },
  { PROGINFOPATH    , "../" CONF_DIR_NAME "/" DB_MAN_PID_FILENAME },
  { PORTNUMBER      , TO_STRING(DESTIN_PORT)                      },
  { SHAPECITYNAME   , SHP_FIELD_NAME                              },
  { SHAPEOSMID      , SHP_OSM_ID                                  },
 };


void InitConfiguration(char const *OwnProgName)
 {
  InitConf(ConfData, NUM_CONF_IT, OwnProgName);  
 }


char const *GetDataBaseFilePathName()
 {
  return GetDataByName(ConfData, NUM_CONF_IT, DATABASEPATH);
 }

char const *GetGeoLocShapeFilePathName()  // GeoLocation Shape Path & Name of the shape file.
 {
  return GetDataByName(ConfData, NUM_CONF_IT, SHAPEFILESPATH);
 }

char const *GetProgInfoPIDFilePathName()  // Path & Name of the file with the program PID.
 {
  return GetDataByName(ConfData, NUM_CONF_IT, PROGINFOPATH);
 }

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

char const *GetColumnNameWithGeolocationName()
 {
  return GetDataByName(ConfData, NUM_CONF_IT, SHAPECITYNAME);
 }

char const *GetColumnNameWithOSMID()
 {
  return GetDataByName(ConfData, NUM_CONF_IT, SHAPEOSMID);
 }

