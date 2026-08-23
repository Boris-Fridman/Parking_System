#ifndef ____Logging_h__
#define ____Logging_h__


#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <sys/types.h>
#include <string.h>
#include "CommonData.h"

/*======================================================================================================================*/
/*----------------------------------------------------------------------------------------------------------------------*/
/* Log Level enumeration. Defines a type of message that is sent to the .log file.                                      */
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

/*----------------------------------------------------------------------------------------------------------------------*/
/* The structure with the logging parameters. Contains information with which the function in the logging library work. */
#ifndef __cplusplus
typedef 
#endif
struct LogParams_s
 {
  bool FileIsOpen;                 /* "true" if the .log file is open or "false" if close.          */
  FILE *fp;                        /* The reference to .log file                                    */
  char LogFilePathName[PATH_LEN];  /* The .log file name including path.                            */
  char DatDiv; /* Data divider */  /* The data divider that is added between logging-message parts. */
 }
#ifndef __cplusplus
LogParams_s
#endif
;

/*----------------------------------------------------------------------------------------------------------------------*/
/* The structure wiht the parameters defining the message wich is sent to the .log file.                                */
#ifndef __cplusplus
typedef 
#endif
struct LogMessType_s
 {
  pid_t ProcessID;                /* The process ID from which the log message is given.                                             */
  pid_t ThreadID;                 /* The thread ID from which the log message is given.                                              */
  time_t TimeOfHappening;         /* The date and time when the log event was heppened.                                              */
  LogLevel_e LogLevel;            /* The type of message given to the logging.                                                       */
  char ProcName[NAME_LEN];        /* The name of the process from which the message is sent. Can be any other description if needed. */
  char LogMessage[LOGMSG_LEN];    /* The message itself to be logged.                                                                */
 }
#ifndef __cplusplus
LogMessType_s
#endif
;

/*======================================================================================================================*/


/**
 * @brief Initilizes the logging including loading the default parameters' values, implemenging the value name to them and creates a .log file with adding the log heather there.
 * 
 * @code
 * void InitLog(LogParams_s *LogParams, char const LogFilePathName[]);
 * @code
 * 
 * @param LogParams The Logging Parameters with which the logging library works.
 * 
 * @param LogFilePathName The name of the .log file including path.
 */
void InitLog(LogParams_s *LogParams, char const LogFilePathName[]);

/**
 * @brief Adds logging message.
 * 
 * @code
 * void AddToLog(LogParams_s *LogParams, LogMessType_s LogMessage);
 * @code
 * 
 * @param LogParams The Logging Parameters with which the logging library works.
 * 
 * @param LogMessage The message added to the log file.
 */
void AddToLog(LogParams_s *LogParams, LogMessType_s LogMessage);

/**
 * @brief Opens .log file.
 * 
 * @code
 * void OpenLog(LogParams_s *LogParams);
 * @code
 * 
 * @param LogParams The Logging Parameters with which the logging library works.
 */
void OpenLog(LogParams_s *LogParams);

/**
 * @brief Closes .log file.
 * 
 * @code
 * void CloseLog(LogParams_s *LogParams);
 * @code
 * 
 * @param LogParams The Logging Parameters with which the logging library works.
 */
void CloseLog(LogParams_s *LogParams);

/**
 * @brief Creates the logging message for logging it. The procedure is separeted from the "AddToLog()" for letting the library to implemment with queueing if needed.
 * 
 * @code
 * LogMessType_s MakeLogMessage(LogLevel_e LogLevel, char const ProcName[], char const Message[]);
 * @code
 * 
 * @param LogLevel The logging message type that can be event, message, attention, warning, error or fail.
 * 
 * @param ProcName The name of the process from which the logging message is sent. Can be also the name of procedure, function or task if needed.
 * 
 * @param Message The message itself that is logged.
 */
LogMessType_s MakeLogMessage(LogLevel_e LogLevel, char const ProcName[], char const Message[]);


#ifdef __cplusplus
}
#endif


#endif    //  ____Logging_h__


