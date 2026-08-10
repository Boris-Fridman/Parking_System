#ifndef ____Logging_h__
#define ____Logging_h__


#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <sys/types.h>
#include <string.h>
#include "CommonData.h"

#ifndef __cplusplus
typedef 
#endif
enum LogLevel_e
 {
  E_LOG_EVENT,     /* Logging Event     */
  E_LOG_MESSAGE,   /* Logging Message   */
  E_LOG_ATTENTION, /* Logging Attention */
  E_LOG_WARNING,   /* Logging Warning   */
  E_LOG_ERROR,     /* Logging Error     */
  E_LOG_FAIL,      /* Logging Fail      */
 }
#ifndef __cplusplus
 LogLevel_e
#endif
;

#ifndef __cplusplus
typedef 
#endif
struct LogParams_s
 {
  bool FileIsOpen;
  FILE *fp;
  char FileName[PATH_LEN];
  char DatDiv; /* Data divider */
 }
#ifndef __cplusplus
LogParams_s
#endif
;



#ifndef __cplusplus
typedef 
#endif
struct LogMessType_s
 {
  pid_t ProcessID;
  pid_t ThreadID;
  time_t TimeOfHappening;
  LogLevel_e LogLevel;
  char ProcName[NAME_LEN];
  char LogMessage[LOGMSG_LEN];
 }
#ifndef __cplusplus
LogMessType_s
#endif
;


void LoadDefParams(LogParams_s *DefParams);

void InitLog(LogParams_s *LogParams, char const FileName[]);

void PrintHeader(LogParams_s *LogParams);

void AddToLog(LogParams_s *LogParams, LogMessType_s LogMessage);

void OpenLog(LogParams_s *LogParams);

void CloseLog(LogParams_s *LogParams);

LogMessType_s MakeLogMessage(LogLevel_e LogLevel, char const ProcName[], char const Message[]);


#ifdef __cplusplus
}
#endif


#endif    //  ____Logging_h__


