#pragma once

#include "main.hpp"


#include "CommonData.h"


enum LogType_e
 {
  LOG_EVENT,
  LOG_WARNING,
  LOG_ERROR,
  LOG_FAIL,
 };


struct LogMessType_s
 {
  LogType_e LogType;
  char LogMessage[LOGMSG_LEN];
 };

