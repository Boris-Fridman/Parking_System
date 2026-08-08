#pragma once

#include "main.hpp"

#include "CommonData.h"


enum LogLevel_e
 {
  LOG_EVENT,     /* Logging Event     */
  LOG_MESSAGE,   /* Logging Message   */
  LOG_ATTENGION, /* Logging Attention */
  LOG_WARNING,   /* Logging Warning   */
  LOG_ERROR,     /* Logging Error     */
  LOG_FAIL,      /* Logging Fail      */
 };

struct LogParams_s
 {
  bool FileIsOpen;
  FILE *fp;
  char FileName[PATH_LEN];
  char DatDiv; /* Data divider */
 };



struct LogMessType_s
 {
  pid_t ProcessID;
  pid_t ThreadID;
  time_t TimeOfHappening;
  ProcTypeID_e ProcType;
  LogLevel_e LogLevel;
  char ProcName[NAME_LEN];
  char LogMessage[LOGMSG_LEN];
 };


void LoadDefParams(LogParams_s *DefParams);

void InitLog(LogParams_s *LogParams, char const FileName[]);

void AddToLog(LogParams_s *LogParams, LogMessType_s LogMessage);

void OpenLog(LogParams_s *LogParams);

void CloseLog(LogParams_s *LogParams);


