
/*======================================================================================================================*/

#include "ConfMan.h"


#include "string.h"

#include <libgen.h> /* Required header for dirname() */


/*======================================================================================================================*/

#define CONF_IT_DIVIDER  ':'  /* The divider betwen key and data in the .ini text file. */

/*======================================================================================================================*/
/*
 * *************************************************************************************************************
 **          Basic configuration managing Functions.
 * *************************************************************************************************************
 */

/*----------------------------------------------------------------------------------------------------------------------*/
/* Scans all the data items and in each key finishing with "PATH" converts existing pathes in data from local to full   */
/* by running the "AdjustPath()" function.                                                                              */
static void CheckAdjustPaths(ConfData_s ConfData[], size_t const NumConfIt, char const *OwnProgName)
 {
  size_t i;
  for(i = 0; i < NumConfIt; i++)
   {
    char *p = strstr(ConfData[i].Key, "PATH");
    if(p != NULL)
     AdjustPath(OwnProgName, ConfData[i].Value, sizeof(ConfData[i].Value));
   }
 }


/*----------------------------------------------------------------------------------------------------------------------*/
/* Loads configuration from file.                                                                                       */
static bool LoadConfFromFile(ConfData_s ConfData[], size_t const NumConfIt, char const *OwnProgName)
 {
  char FileName[PATH_LEN];
  size_t i;
  char line[KEY_LEN + VAL_LEN + 10];
  FILE *ConfIniFile;
  char *StartConfInf;
  char StrToFind[KEY_LEN + 2];
  char *lastc;
  size_t NumFoundOptions;

  GetConfFileName(OwnProgName, FileName, sizeof(FileName));
  printf("%s\n\r", FileName);
  
  ConfIniFile = fopen(FileName, "r");

  if(ConfIniFile)
   {
    NumFoundOptions = 0;
    while(fgets(line, sizeof(line), ConfIniFile) != NULL)
     {
      lastc = &line[strlen(line) - 1];
      if((*lastc == '\n') || (*lastc == '\r'))
       *lastc = '\0';
      printf("%s\n\r", line);
      for(i = 0; i < NumConfIt; i++)
       {
        volatile ssize_t ItSize = sizeof(StrToFind);  // For preventing warning.
        snprintf(StrToFind, ItSize - 2, "%s%c", ConfData[i].Key, CONF_IT_DIVIDER);
        StartConfInf = strstr(line, StrToFind);
        if((StartConfInf != NULL) && (strlen(StartConfInf) > 0))  /* Attention !!! Due to the shortcircuit characteristics the conditions MUSTN'T be changed places. */
         {
          strncpy(ConfData[i].Value, &StartConfInf[strlen(StrToFind)], (sizeof(ConfData[i].Value) - 1));
          NumFoundOptions++;
         }
       }
     }
    fclose(ConfIniFile);
    return NumFoundOptions == NumConfIt;
   }
  else  
   return false;
 }

// #include <unistd.h>
// #include <limits.h>
/*----------------------------------------------------------------------------------------------------------------------*/
/* Creates a default configuration.                                                                                     */
static bool CreateDefConf(ConfData_s ConfData[], size_t const NumConfIt, char const *OwnProgName)
 {
  char FileName[PATH_LEN];
  size_t i;
  FILE *ConfIniFile;

  GetConfFileName(OwnProgName, FileName, sizeof(FileName));
  printf("%s\n\r", FileName);

  ConfIniFile = fopen(FileName, "w");
  if(ConfIniFile != NULL)
   {
    for(i = 0; i < NumConfIt; i++)
     {
      fprintf(ConfIniFile, "%s%c%s\n", ConfData[i].Key, CONF_IT_DIVIDER, ConfData[i].Value);
     }
    fclose(ConfIniFile);
    printf("The configuration file was created successfylly.\n\r");
    return true;
   }
  else
   {
    printf("Couldn't create the configuration file.\n\r");
    return false;
   }
 }


/*======================================================================================================================*/
/*
 * *************************************************************************************************************
 **          Main configuration managing Functions.
 * *************************************************************************************************************
 */

/*----------------------------------------------------------------------------------------------------------------------*/
/* Initilizes configuration.                                                                                            */
void InitConf(ConfData_s ConfData[], size_t const NumConfIt, char const *OwnProgName)
 {
  bool Result;
  
  Result = LoadConfFromFile(ConfData, NumConfIt, OwnProgName);
  if(!Result)
   {
    CreateDefConf(ConfData, NumConfIt, OwnProgName);
   }

  CheckAdjustPaths(ConfData, NumConfIt, OwnProgName);
 }


/*----------------------------------------------------------------------------------------------------------------------*/
/* Gives data by a given name.                                                                                          */
char const *GetDataByName(ConfData_s ConfData[], size_t const NumConfIt, char const DataName[])
 {
  size_t i;
  for(i = 0; i < NumConfIt; i++)
   {
    if(!strcmp(ConfData[i].Key, DataName))
     {
      return ConfData[i].Value;
     }
   }
  return NULL;
 }

