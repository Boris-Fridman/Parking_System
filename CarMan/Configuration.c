#include "Configuration.h"

#include <string.h>

#include <libgen.h> // Required header for dirname()
#include "ConfMan.h"



#define CONFIGPATH           "CONFIGPATH"
#define LOGFILEPATH          "LOGFILEPATH"
#define DHCPENSTAT           "DHCPENSTAT"
#define DHCPNAME             "DHCPNAME"
#define IPADDRESS            "IPADDRESS"
#define PORTNUMBER           "PORTNUMBER"
#define CLIENTNAME           "CLIENTNAME"
#define VECHICLEID           "VECHICLEID"
#define MAXPARKTIME          "MAXPARKTIME"
#define MAXWAITPARKDELAY     "MAXWAITPARKDELAY"


#define NUM_CONF_IT   10
char ConfData[NUM_CONF_IT][2][PATH_LEN] = 
 {
  { CONFIGPATH            , "../" CONF_DIR_NAME                         },
  { LOGFILEPATH           , "../" CONF_DIR_NAME "/" LOG_CAR_FILENAME    },
  { DHCPENSTAT            , TO_STRING(0)                                },
  { DHCPNAME              , ""                                          },
  { IPADDRESS             , DESTIN_IP                                   },
  { PORTNUMBER            , TO_STRING(DESTIN_PORT)                      },  
  { CLIENTNAME            , DEF_CLIENT_NAME                             },
  { VECHICLEID            , TO_STRING(DEF_VEHICLE_ID)                   },  
  { MAXPARKTIME           , TO_STRING(DEF_MAX_PARK_TIME)                },
  { MAXWAITPARKDELAY      , TO_STRING(DEF_MAX_PARK_DELAY)               },
 };


void InitConfiguration(char const *OwnProgName)
 {
  InitConf(ConfData, NUM_CONF_IT, OwnProgName);  
 }


char const *GetLogFilePathName()          // Path & Name of the file with the program log.
 {
  return GetDataByName(ConfData, NUM_CONF_IT, LOGFILEPATH);
 }

bool GetUseDHCPState()
 {
  char const *UseDHCPState = GetDataByName(ConfData, NUM_CONF_IT, DHCPENSTAT);
  return (UseDHCPState[0] != '\0') && (UseDHCPState[0] != '0');
 }

char const *GetDestinDHCPName()
 {
  return GetDataByName(ConfData, NUM_CONF_IT, DHCPNAME);
 }

char const *GetDestinAddr()
{
 return GetDataByName(ConfData, NUM_CONF_IT, IPADDRESS);
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

char const *GetClientName()
 {
  return GetDataByName(ConfData, NUM_CONF_IT, CLIENTNAME);
 }

uint32_t GetVechicleID()
 {
  uint32_t Result;
  char *endptr;
  char const *VechIDAsStr = GetDataByName(ConfData, NUM_CONF_IT, VECHICLEID);
  Result = strtoll(VechIDAsStr, &endptr, 10);
  if(endptr == VechIDAsStr)
   return DEF_VEHICLE_ID;
  else
   return Result;
 }

uint32_t GetMaxParkTime()
 {
  uint32_t Result;
  char *endptr;
  char const *Value = GetDataByName(ConfData, NUM_CONF_IT, MAXPARKTIME);
  Result = strtoll(Value, &endptr, 10);
  if(endptr == Value)
   return DEF_MAX_PARK_TIME;
  else
   return Result;
 }

uint32_t GetMaxParkWaitTime()
 {
  uint32_t Result;
  char *endptr;
  char const *Value = GetDataByName(ConfData, NUM_CONF_IT, MAXWAITPARKDELAY);
  Result = strtoll(Value, &endptr, 10);
  if(endptr == Value)
   return DEF_MAX_PARK_DELAY;
  else
   return Result;
 }








