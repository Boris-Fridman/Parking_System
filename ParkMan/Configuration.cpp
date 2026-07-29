#include "Configuration.hpp"

#include "iostream"
#include "fstream"
#include "cstring"

#include <libgen.h> // Required header for dirname()




#define CONFIGPATH     "CONFIGPATH"
#define DATABASEPATH   "DATABASEPATH"
#define SHAPEFILESPATH "SHAPEFILESPATH"
#define PROGINFOPATH   "PROGINFOPATH"
#define PORTNUMBER     "PORTNUMBER"
#define SHAPECITYNAME  "SHAPECITYNAME"


#define CONF_IT_DIVIDER  ':'

#define NUM_CONF_IT   6
char ConfData[NUM_CONF_IT][2][PATH_LEN] = 
 {
  { CONFIGPATH      , "../" CONF_DIR_NAME                         },
  { DATABASEPATH    , "../" CONF_DIR_NAME "/" DB_FILENAME         },
  { SHAPEFILESPATH  , "../" CONF_DIR_NAME "/" SHP_FILENAME        },
  { PROGINFOPATH    , "../" CONF_DIR_NAME "/" DB_MAN_PID_FILENAME },
  { PORTNUMBER      , TO_STRING(DESTIN_PORT)                      },
  { SHAPECITYNAME   , SHP_FIELD_NAME                              },
 };




bool LoadConfigFromFile(char const *OwnProgName)
 {
  char FileName[PATH_LEN];
  ssize_t i;
  std::string line;
  size_t SymbPos;
  int len;
  

  GetConfFileName(OwnProgName, FileName);
  std::cout << FileName << "\n\r";
  
  std::ifstream ConfIniFile(FileName);
  
  if(ConfIniFile.is_open())
   {
    while(std::getline(ConfIniFile, line))
     {
      std::cout << line << "\n\r";
      for(i = 0; i < NUM_CONF_IT; i++)
       {
        SymbPos = line.find(std::string(ConfData[i][0]) + CONF_IT_DIVIDER);
        if(SymbPos != std::string::npos)
         {
          len = strlen(ConfData[i][0]);
          strcpy(ConfData[i][1], &line[len + 1]);
          char *p = strstr(ConfData[i][0], "PATH");
          if(p != nullptr)
           AdjustPath(OwnProgName, ConfData[i][1]);
         }
       }
     }
    ConfIniFile.close();
    return true;
   }
  else  
   return false;
 }

bool CreateDefConf(char const *OwnProgName)
 {
  char FileName[PATH_LEN];
  ssize_t i;

  GetConfFileName(OwnProgName, FileName);
  std::cout << FileName << "\n\r";

  std::ofstream ConfIniFile(FileName);

  if(ConfIniFile.is_open())
   {
    for(i = 0; i < NUM_CONF_IT; i++)
     {
      ConfIniFile << ConfData[i][0] << CONF_IT_DIVIDER << ConfData[i][1] << "\n";
     }
    ConfIniFile << std::endl;
    ConfIniFile.close();
    return true;
   }
  else
   return false;
 }


void InitConf(char const *OwnProgName)
 {
  bool Result;
  
  Result = LoadConfigFromFile(OwnProgName);
  if(!Result)
   {
    CreateDefConf(OwnProgName);
   }
  
 }

char const *GetDataByName(char const DataName[])
 {
  ssize_t i;
  for(i = 0; i < NUM_CONF_IT; i++)
   {
    if(!strcmp(ConfData[i][0], DataName))
     {
      return ConfData[i][1];
     }
   }
  return nullptr;
 }

char const *GetDataBaseFilePathName()
 {
  return GetDataByName(DATABASEPATH);
 }

char const *GetGeoLocShapeFilePathName()  // GeoLocation Shape Path & Name of the shape file.
 {
  return GetDataByName(SHAPEFILESPATH);
 }

char const *GetProgInfoPIDFilePathName()  // Path & Name of the file with the program PID.
 {
  return GetDataByName(PROGINFOPATH);
 }

uint16_t GetDestinPort()
 {
  int Result;
  char *endptr;
  char const *PortNumStr = GetDataByName(PORTNUMBER);
  Result = strtol(PortNumStr, &endptr, 10);
  if(endptr == PortNumStr)
   return DESTIN_PORT;
  else
   return Result;
 }

char const *GetColumnNameWithGeolocationName()
 {
  return GetDataByName(SHAPECITYNAME);
 }


