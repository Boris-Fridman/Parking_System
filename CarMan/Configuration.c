#include "Configuration.h"

#include "string.h"

#include <libgen.h> // Required header for dirname()




#define CONFIGPATH     "CONFIGPATH"
#define DHCPENSTAT     "DHCPENSTAT"
#define DHCPNAME       "DHCPNAME"
#define IPADDRESS      "IPADDRESS"
#define PORTNUMBER     "PORTNUMBER"
#define CLIENTNAME     "CLIENTNAME"
#define VECHICLEID     "VECHICLEID"


#define CONF_IT_DIVIDER  ':'

#define NUM_CONF_IT   7
char ConfData[NUM_CONF_IT][2][PATH_LEN] = 
 {
  { CONFIGPATH      , "../" CONF_DIR_NAME                         },
  { DHCPENSTAT      , TO_STRING(0)                                },
  { DHCPNAME        , ""                                          },
  { IPADDRESS       , DESTIN_IP                                   },
  { PORTNUMBER      , TO_STRING(DESTIN_PORT)                      },  
  { CLIENTNAME      , DEF_CLIENT_NAME                             },
  { VECHICLEID      , TO_STRING(DEF_VEHICLE_ID)                   },  
 };




bool LoadConfigFromFile(char const *OwnProgName)
 {
  char FileName[PATH_LEN];
  ssize_t i;
  char line[(PATH_LEN) * 2 + 1];
  FILE *ConfIniFile;
  char *StartConfInf;
  char StrToFind[PATH_LEN];
  char *lastc;
  

  GetConfFileName(OwnProgName, FileName);
  printf("%s\n\r", FileName);
  
  ConfIniFile = fopen(FileName, "r");

  if(ConfIniFile)
   {
    while(fgets(line, sizeof(line), ConfIniFile) != NULL)
     {
      lastc = &line[strlen(line) - 1];
      if((*lastc == '\n') || (*lastc == '\r'))
       *lastc = '\0';
      printf("%s\n\r", line);
      for(i = 0; i < NUM_CONF_IT; i++)
       {
        snprintf(StrToFind, sizeof(StrToFind) - 2, "%s%c", ConfData[i][0], CONF_IT_DIVIDER);
        StartConfInf = strstr(line, StrToFind);
        if((StartConfInf != NULL) && (strlen(StartConfInf) > 0))  /* Attention !!! Due to the shortcircuit characteristics the conditions MUSTN'T be changed places. */
         {
          strcpy(ConfData[i][1], &StartConfInf[strlen(StrToFind)]);
          char *p = strstr(ConfData[i][0], "PATH");
          if(p != NULL)
           AdjustPath(OwnProgName, ConfData[i][1]);
         }
       }
     }
    fclose(ConfIniFile);
    return true;
   }
  else  
   return false;
 }

bool CreateDefConf(char const *OwnProgName)
 {
  char FileName[PATH_LEN];
  ssize_t i;
  FILE *ConfIniFile;

  GetConfFileName(OwnProgName, FileName);
  printf("%s\n\r", FileName);

  ConfIniFile = fopen(FileName, "w");
  if(ConfIniFile != NULL)
   {
    for(i = 0; i < NUM_CONF_IT; i++)
     {
      fprintf(ConfIniFile, "%s%c%s\n", ConfData[i][0], CONF_IT_DIVIDER, ConfData[i][1]);
     }
    fclose(ConfIniFile);
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
  return NULL;
 }


bool GetUseDHCPState()
 {
  char const *UseDHCPState = GetDataByName(DHCPENSTAT);
  return (UseDHCPState[0] != '\0') || (UseDHCPState[0] != '0');
 }

char const *GetDestinDHCPName()
 {
  return GetDataByName(DHCPNAME);
 }

char const *GetDestinAddr()
{
 return GetDataByName(IPADDRESS);
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

char const *GetClientName()
 {
  return GetDataByName(CLIENTNAME);
 }

uint32_t GetVechicleID()
 {
  uint32_t Result;
  char *endptr;
  char const *VechIDAsStr = GetDataByName(VECHICLEID);
  Result = strtoll(VechIDAsStr, &endptr, 10);
  if(endptr == VechIDAsStr)
   return DEF_VEHICLE_ID;
  else
   return Result;

 }










