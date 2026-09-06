#include "Logging.h"
#include <unistd.h>
#include <time.h>
#include <string.h>

/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Logging control additional functions for internal usage only.
 * *************************************************************************************************************
 */

/*----------------------------------------------------------------------------------------------------------------------*/
/* Returns the log message name according the given enumeration signaliaing it.                                         */
static char const *LogLevelName(LogLevel_e LogLevel)
 {
  switch(LogLevel)
   {
    case E_LOG_EVENT:     /* Logging Event   */
      return "Event    ";
     break;
    case E_LOG_MESSAGE:   /* Logging Message */
      return "Message  ";
     break;
    case E_LOG_ATTENTION: /* Logging Attention */
      return "Attention";
     break;
    case E_LOG_WARNING:   /* Logging Warning */
      return "Warning  ";
     break;
    case E_LOG_ERROR:     /* Logging Error   */
      return "Error    ";
     break;
    case E_LOG_FAIL:      /* Logging Fail    */    
      return "Fail     ";
     break;
    default:
     return "Unknown  ";
     break;
   }
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Creates the strig line to log.                                                                                       */
static void CreateStringLineToLog(LogParams_s const * const LogParams, char LogString[], size_t MaxSize, LogMessType_s LogMessage)
 {
  char TimeBuf[40];
  size_t NumMissingChars, Len;
  ConvertTime(&LogMessage.TimeOfHappening, TimeBuf, sizeof(TimeBuf), E_DBS_FORMAT);
  Len = strlen(LogMessage.ProcName);
  if(Len < LogParams->MinNameLen)
   {
    NumMissingChars = LogParams->MinNameLen - Len;
    for(size_t i = 0; i < NumMissingChars; i++)
     {
      strncat(LogMessage.ProcName, " ", sizeof(LogMessage.ProcName) - 1);
     }
   }

  snprintf(LogString, MaxSize - 2, "%s%c%d%c%d%c%s%c%s%c%s", TimeBuf, LogParams->DatDiv, LogMessage.ProcessID, LogParams->DatDiv, LogMessage.ThreadID, LogParams->DatDiv, LogMessage.ProcName, LogParams->DatDiv, LogLevelName(LogMessage.LogLevel), LogParams->DatDiv, LogMessage.LogMessage);
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Prings the logging header to a .log file.                                                                            */
static void PrintHeader(LogParams_s *LogParams)
 {
  fprintf(LogParams->fp, "\n     Date          Time  %c PID  %c TID  %cProc Name%cLog Level%c     Message\n", LogParams->DatDiv, LogParams->DatDiv, LogParams->DatDiv, LogParams->DatDiv, LogParams->DatDiv);
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Sets the params in the given LogParams_s structure to the default values.                                            */
static void LoadDefParams(LogParams_s *DefParams)
 {
  memset(DefParams, 0, sizeof(LogParams_s));
  DefParams->DatDiv = '\t';
  DefParams->MinNameLen = MIN_PROC_NAME_LEN;
 }

/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Logging control main functions to be called externally.
 * *************************************************************************************************************
 */


/*----------------------------------------------------------------------------------------------------------------------*/
/* Initilizes the logging including loading the default parameters' values, implemenging the value name                 */
/* to them and creates a .log file with adding the log heather there.                                                   */
void InitLog(LogParams_s *LogParams, char const LogFilePathName[])
 {
  LoadDefParams(LogParams);
  strncpy(LogParams->LogFilePathName, LogFilePathName, sizeof(LogParams->LogFilePathName) - 1);

  OpenLog(LogParams);
  PrintHeader(LogParams);
  CloseLog(LogParams);
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Opens .log file.                                                                                                     */
void OpenLog(LogParams_s *LogParams)
 {
  if(!LogParams->FileIsOpen)
   {
    LogParams->fp = fopen(LogParams->LogFilePathName, "a");
    if(LogParams->fp == NULL)
     {
      printf("Error opening ifle!");
     }
    else 
     {
      LogParams->FileIsOpen = true;
     }
   }
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Closes .log file.                                                                                                    */
void CloseLog(LogParams_s *LogParams)
 {
  if(LogParams->FileIsOpen)
   {
    fflush(LogParams->fp);       
    fclose(LogParams->fp);
    LogParams->FileIsOpen = false;
   }
 }


/*----------------------------------------------------------------------------------------------------------------------*/
/* Adds logging message.                                                                                                */
void AddToLog(LogParams_s *LogParams, LogMessType_s LogMessage)
 {
  char Buf[LOGMSG_LEN + 100];
  CreateStringLineToLog(LogParams, Buf, sizeof(Buf), LogMessage);
  fprintf(LogParams->fp, "%s\n", Buf);
  fflush(LogParams->fp);
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Creates the logging message for logging it.                                                                          */
/* The procedure is separeted from the "AddToLog()" for letting the library to implemment with queueing if needed.      */
LogMessType_s MakeLogMessage(LogLevel_e LogLevel, char const ProcName[], char const Message[])
 {
  LogMessType_s Result;
  memset(&Result, 0, sizeof(LogMessType_s));
  Result.LogLevel = LogLevel;
  Result.ProcessID = getpid();
  Result.ThreadID = gettid();
  time(&Result.TimeOfHappening);
  strncpy(Result.ProcName, ProcName, sizeof(Result.ProcName)-1);
  strncpy(Result.LogMessage, Message, sizeof(Result.LogMessage) - 1);
  return Result;
 }

