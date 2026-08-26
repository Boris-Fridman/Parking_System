#include "Configuration.h"

#include <string.h>

#include <libgen.h> // Required header for dirname()
#include "ConfMan.h"


/*======================================================================================================================*/

#define CONFIGPATH           "CONFIGPATH"                                   /* Path to folder with configuration data.                                                                                                                         */
#define LOGFILEPATH          "LOGFILEPATH"                                  /* Path to log file.                                                                                                                                               */
#define DHCPENSTAT           "DHCPENSTAT"                                   /* Option if connect to the cliend by DHCP name or by static IP address. "0" - Static IP Address "1" - DHCP Server Name.                                           */
#define DHCPNAME             "DHCPNAME"                                     /* DHCP Server name if theclient must use it.                                                                                                                      */
#define IPADDRESS            "IPADDRESS"                                    /* Static IP Address if the client must use it.                                                                                                                    */
#define PORTNUMBER           "PORTNUMBER"                                   /* Port number that is opened during running the program for receiving information sent from clients.                                                              */
#define CLIENTNAME           "CLIENTNAME"                                   /* The name of the client that is sent via the network via the network during parking with other client parking information.                                       */
#define VECHICLEID           "VECHICLEID"                                   /* Vehicle ID that is sent via the network wiht the other client information.                                                                                      */
#define MAXPARKTIME          "MAXPARKTIME"                                  /* The maximal time of parking (in seconds) that can be generated randomly for parking emulation. If set to "0" the parking will be infinit without interruptings. */
#define MAXWAITPARKDELAY     "MAXWAITPARKDELAY"                             /* The maximal time of pause between the parkings (in seconds) that can be generated randomly for parking emulation.                                               */


#define NUM_CONF_IT           10                                            /* The number of items existing in the confituration ini file.                                                                                                     */
char ConfData[NUM_CONF_IT][2][PATH_LEN] =                                   /* The Array of string-pairs containing the configuration items when the first string of pair is a key na dthe second one is the data.                             */
 {
  { CONFIGPATH            , "../" CONF_DIR_NAME                         },  /* Path to folder with configuration data.                                                                                                                         */
  { LOGFILEPATH           , LOG_CAR_FILENAME                            },  /* Path to log file.                                                                                                                                               */
  { DHCPENSTAT            , TO_STRING(0)                                },  /* Option if connect to the cliend by DHCP name or by static IP address. "0" - Static IP Address "1" - DHCP Server Name.                                           */
  { DHCPNAME              , ""                                          },  /* DHCP Server name if theclient must use it.                                                                                                                      */
  { IPADDRESS             , DESTIN_IP                                   },  /* Static IP Address if the client must use it.                                                                                                                    */
  { PORTNUMBER            , TO_STRING(DESTIN_PORT)                      },  /* Port number that is opened during running the program for receiving information sent from clients.                                                              */
  { CLIENTNAME            , DEF_CLIENT_NAME                             },  /* The name of the client that is sent via the network via the network during parking with other client parking information.                                       */
  { VECHICLEID            , TO_STRING(DEF_VEHICLE_ID)                   },  /* Vehicle ID that is sent via the network wiht the other client information.                                                                                      */
  { MAXPARKTIME           , TO_STRING(DEF_MAX_PARK_TIME)                },  /* The maximal time of parking (in seconds) that can be generated randomly for parking emulation. If set to "0" the parking will be infinit without interruptings. */
  { MAXWAITPARKDELAY      , TO_STRING(DEF_MAX_PARK_DELAY)               },  /* The maximal time of pause between the parkings (in seconds) that can be generated randomly for parking emulation.                                               */
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
/* Returns the path with name of the file containing the logging information.                                           */
char const *GetLogFilePathName()          /* Path & Name of the file with the program log. */
 {
  return GetDataByName(ConfData, NUM_CONF_IT, LOGFILEPATH);
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Returns "1" if the DHCP is enabled in the .ini file or "0" if not.                                                   */
bool GetUseDHCPState()
 {
  char const *UseDHCPState = GetDataByName(ConfData, NUM_CONF_IT, DHCPENSTAT);
  return (UseDHCPState[0] != '\0') && (UseDHCPState[0] != '0');
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Returns the DHCP Name of the server.                                                                                 */
char const *GetDestinDHCPName()
 {
  return GetDataByName(ConfData, NUM_CONF_IT, DHCPNAME);
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Returns the static IP Address of the server.                                                                         */
char const *GetDestinAddr()
{
 return GetDataByName(ConfData, NUM_CONF_IT, IPADDRESS);
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
/* Returns the Name of the client for parking information.                                                              */
char const *GetClientName()
 {
  return GetDataByName(ConfData, NUM_CONF_IT, CLIENTNAME);
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Returns the Vehicle ID of the client for parking information.                                                        */
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

/*----------------------------------------------------------------------------------------------------------------------*/
/* Returns the maximal parking time that can be randomely generated or "0" if it is infinit.                            */
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

/*----------------------------------------------------------------------------------------------------------------------*/
/* Returns the maximal pause time between the parkings that can be randomely generated.                                 */
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








