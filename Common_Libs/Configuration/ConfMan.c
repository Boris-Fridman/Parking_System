#include "ConfMan.h"


#include "string.h"

#include <libgen.h> // Required header for dirname()


#define CONF_IT_DIVIDER  ':'

static bool LoadConfFromFile(char ConfData[][2][PATH_LEN], ssize_t NumConfIt,char const *OwnProgName)
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
      for(i = 0; i < NumConfIt; i++)
       {
        volatile ssize_t ItSize = PATH_LEN;  // For preventing warning.
        snprintf(StrToFind, ItSize - 2, "%s%c", ConfData[i][0], CONF_IT_DIVIDER);
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

static bool CreateDefConf(char ConfData[][2][PATH_LEN], ssize_t NumConfIt, char const *OwnProgName)
 {
  char FileName[PATH_LEN];
  ssize_t i;
  FILE *ConfIniFile;

  GetConfFileName(OwnProgName, FileName);
  printf("%s\n\r", FileName);

  ConfIniFile = fopen(FileName, "w");
  if(ConfIniFile != NULL)
   {
    for(i = 0; i < NumConfIt; i++)
     {
      fprintf(ConfIniFile, "%s%c%s\n", ConfData[i][0], CONF_IT_DIVIDER, ConfData[i][1]);
     }
    fclose(ConfIniFile);
    return true;
   }
  else
   return false;
 }



void InitConf(char ConfData[][2][PATH_LEN], ssize_t NumConfIt, char const *OwnProgName)
 {
  bool Result;
  
  Result = LoadConfFromFile(ConfData, NumConfIt, OwnProgName);
  if(!Result)
   {
    CreateDefConf(ConfData, NumConfIt, OwnProgName);
   }
  
 }


char const *GetDataByName(char ConfData[][2][PATH_LEN], ssize_t NumConfIt, char const DataName[])
 {
  ssize_t i;
  for(i = 0; i < NumConfIt; i++)
   {
    if(!strcmp(ConfData[i][0], DataName))
     {
      return ConfData[i][1];
     }
   }
  return NULL;
 }

