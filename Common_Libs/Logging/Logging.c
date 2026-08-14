#include "Logging.h"
#include <unistd.h>
#include <time.h>
#include <string.h>


char const *LogLevelName(LogLevel_e LogLevel)
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

void LoadDefParams(LogParams_s *DefParams)
 {
  memset(DefParams, 0, sizeof(LogParams_s));
  DefParams->DatDiv = '\t';
 }

void InitLog(LogParams_s *LogParams, char const FileName[])
 {
  LoadDefParams(LogParams);
  strncpy(LogParams->FileName, FileName, sizeof(LogParams->FileName) - 1);

  OpenLog(LogParams);
  PrintHeader(LogParams);
  CloseLog(LogParams);
 }


void OpenLog(LogParams_s *LogParams)
 {
  if(!LogParams->FileIsOpen)
   {
    LogParams->fp = fopen(LogParams->FileName, "a");
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

void CloseLog(LogParams_s *LogParams)
 {
  if(LogParams->FileIsOpen)
   {
    fflush(LogParams->fp);       
    fclose(LogParams->fp);
    LogParams->FileIsOpen = false;
   }
 }
void PrintHeader(LogParams_s *LogParams)
 {
  fprintf(LogParams->fp, "     Date          Time  %c PID  %c TID  %cProc Name%cLog Level%c     Message\n", LogParams->DatDiv, LogParams->DatDiv, LogParams->DatDiv, LogParams->DatDiv, LogParams->DatDiv);
 }

void CreateStringLineToLog(char LogString[], size_t MaxSize, LogMessType_s LogMessage, char Divider)
 {
  char TimeBuf[40];
  ConvertTime(&LogMessage.TimeOfHappening, TimeBuf, sizeof(TimeBuf), E_DBS_FORMAT);
  snprintf(LogString, MaxSize - 2, "%s%c%d%c%d%c%s%c%s%c%s", TimeBuf, Divider, LogMessage.ProcessID, Divider, LogMessage.ThreadID, Divider, LogMessage.ProcName, Divider, LogLevelName(LogMessage.LogLevel), Divider, LogMessage.LogMessage);
 }


void AddToLog(LogParams_s *LogParams, LogMessType_s LogMessage)
 {
  char Buf[LOGMSG_LEN + 70];
  CreateStringLineToLog(Buf, sizeof(Buf), LogMessage, LogParams->DatDiv);
  fprintf(LogParams->fp, "%s\n", Buf);
  fflush(LogParams->fp);
 }



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