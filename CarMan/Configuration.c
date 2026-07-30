#include "Configuration.h"

#include <string.h>

#include <libgen.h> // Required header for dirname()
#include "ConfMan.h"



#define CONFIGPATH     "CONFIGPATH"
#define DHCPENSTAT     "DHCPENSTAT"
#define DHCPNAME       "DHCPNAME"
#define IPADDRESS      "IPADDRESS"
#define PORTNUMBER     "PORTNUMBER"
#define CLIENTNAME     "CLIENTNAME"
#define VECHICLEID     "VECHICLEID"


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


void InitConfiguration(char const *OwnProgName)
 {
  InitConf(ConfData, NUM_CONF_IT, OwnProgName);  
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










