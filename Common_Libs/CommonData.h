
#ifndef ____CommonData_h__
#define ____CommonData_h__

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstdbool>
#else
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#endif

#include <netdb.h>

/*======================================================================================================================*/

/**
 * Terminal Colors
 *
 * The information about colors can be found in the sine:
 * https://en.wikipedia.org/wiki/ANSI_escape_code
 */
/*----------------------------------------------------------------------------------------------------------------------*/
/*  Foreground Colors                                                                                                   */
#define TermBlack           "\033[30m"
#define TermRed             "\033[31m"
#define TermGreen           "\033[32m"
#define TermYello           "\033[33m"
#define TermBlue            "\033[34m"
#define TermMagenta         "\033[35m"
#define TermCyan            "\033[36m"
#define TermWhite           "\033[37m"  /* (Light Gray) */

#define TermBrightBlack     "\033[90m"  /* (Dark Gray)  */
#define TermBrightRed       "\033[91m"
#define TermBrightGreen     "\033[92m"
#define TermBrightYello     "\033[93m"
#define TermBrightBlue      "\033[94m"
#define TermBrightMagenta   "\033[95m"
#define TermBrightCyan      "\033[96m"
#define TermBrightWhite     "\033[97m"

/*----------------------------------------------------------------------------------------------------------------------*/
/*  Background Colors                                                                                                   */
#define TermBGBlack         "\033[40m"
#define TermBGRed           "\033[41m"
#define TermBGGreen         "\033[42m"
#define TermBGYello         "\033[43m"
#define TermBGBlue          "\033[44m"
#define TermBGMagenta       "\033[45m"
#define TermBGCyan          "\033[46m"
#define TermBGWhite         "\033[47m"   /* (Light Gray) */

#define TermBGBrightBlack   "\033[100m"  /* (Dark Gray)  */
#define TermBGBrightRed     "\033[101m"
#define TermBGBrightGreen   "\033[102m"
#define TermBGBrightYello   "\033[103m"
#define TermBGBrightBlue    "\033[104m"
#define TermBGBrightMagenta "\033[105m"
#define TermBGBrightCyan    "\033[106m"
#define TermBGBrightWhite   "\033[107m"

#define TermColorsReset   "\033[39;49m"



/*======================================================================================================================*/

/**
 * Min / Max macros
 */
#define MAX(X, Y)                 ( (X) > (Y) ? (X) : (Y) )           /* The macro returning the biggest from the two values.  */
#define MIN(X, Y)                 ( (X) < (Y) ? (X) : (Y) )           /* The macro returning the smallest from the two values. */

/**
 * Div macros
 */
#define DIV_RND(X,Y)              ( ((X) + (Y) / 2) / (Y) )     /* Deviation with rounding.      10/6 will give 2 and 10/3 will give 3  */
#define DIV_RND_UP(X,Y)           ( ((X) + (Y) - 1) / (Y) )     /* Deviation with rounding up.   10/6 will give 2 and 10/3 will give 4  */
#define DIV_RND_DN(X,Y)           ( (X) / (Y)             )     /* Deviation with rounding down. 10/6 will give 1 and 10/3 will give 3 Regular deviation equal to regular "/". Is defined for compilation the previous macros deviations */

#define SQR(X)                    ((X)*(X))                     /* Rases number to the square power.                                    */




//#define DESTIN_IP               "127.0.0.1"    // For test only. In the future will be removed.
#define DESTIN_IP                 "192.168.1.164"            /* Server IP address to which are sent the call messages. */
#define DESTIN_PORT                8080                      /* Server port to which are sent the GPS messages. */
#define BUFFER_SIZE                1024                      /* The length in bytes, of the buffer pointed by the buf parameter that is used by the recv() function. */

#define CRC_SIZE                  ( sizeof(uint32_t) )



#define NAME_LEN                  30
#define PATH_LEN                  300


#define DB_FILENAME               "ParkingInfo.sqlite3"
#define CONF_DIR_NAME             "ConfDir"
#define DB_MAN_PID_FILENAME       "DB_MAN_PID"
#define DB_UPADATE_SIGNAL         SIGUSR1

/*======================================================================================================================*/

/**
 * Macro for preventing warnings in case of unused variables.
 */
#if !defined(UNUSED)
#define UNUSED(X)                 (void)X      /* To avoid gcc/g++ warnings */
#endif /* UNUSED */

/*======================================================================================================================*/
#ifndef __cplusplus
typedef 
#endif
enum Error_Results_e
 {
  E_FAIL,
  E_CORRECT,
  E_WARNING,
  E_SUCCESS,
  E_PROBLEM
 }
 #ifndef __cplusplus
 Error_Results_e
 #endif
 ;

/* GPS Coordinates. Are given in degrees. */
#ifndef __cplusplus
typedef 
#endif
struct GPS_Cords_s
 {
  double Latitude;    /* (˚) (deg) */  /* Positive number - North , Negative nomber - South .     > 0 - NL ;  < 0 - SL .  */
  double Longitude;   /* (˚) (deg) */  /* Positive number - East  , Negative number - West  .     > 0 - EL ;  < 0 - WL .  */
 }
 #ifndef __cplusplus
 GPS_Cords_s
 #endif
 ;

/* Distance Coordinates are the analogues of the GPS Coordinates and are used for calculating the distance. Are given in kilomters. */
#ifndef __cplusplus
typedef 
#endif
struct Dist_Cords_s
 {
  double x; /* Longitude; */ /* km */ /* Positive number - East  , Negative number - West  .  */
  double y; /* Latitude;  */ /* km */ /* Positive number - North , Negative nomber - South .  */
 }
 #ifndef __cplusplus
 Dist_Cords_s
 #endif
 ;

#ifndef __cplusplus
typedef 
#endif
struct Space_Cords_s
 {
  double x; /* km */
  double y; /* km */
  double z; /* km */
 }
 #ifndef __cplusplus
 Space_Cords_s
 #endif
 ;




 #ifndef __cplusplus
 typedef 
 #endif
 struct Parking_s
  {
   GPS_Cords_s Cords;
   uint16_t    Parking_ID;
   uint16_t    MaxNumPlaces;
   uint16_t    NumPlaces;
   uint16_t    Price;              /*  0.01₪ / hour  */
   char Name[NAME_LEN];
  }
#ifndef __cplusplus  
Parking_s
#endif
;

 #ifndef __cplusplus
typedef 
 #endif
struct PriceTab_s
 {
  uint16_t City_ID;
  uint16_t Price;              /*  0.01₪ / hour  */
  char     City_Name[NAME_LEN];
 }
#ifndef __cplusplus  
PriceTab_s
#endif
;


 #ifndef __cplusplus
 typedef 
 #endif
 struct Customer_s
  {
   GPS_Cords_s Cords;
   uint32_t    Vechicle_ID;
   char        Name[NAME_LEN];
  }
#ifndef __cplusplus  
Customer_s
#endif
;

#ifndef __cplusplus
typedef 
#endif
struct CustAcknowledge_s
 {
  uint32_t Vechicle_ID;
  uint16_t City_ID;
  time_t   ParkingStartTime;
  time_t   ParkingDurationTime;           /* seconds */
  uint16_t AccumulatedPrice;              /*  0.01₪  */
  char     City_Name[NAME_LEN];
 }
#ifndef __cplusplus  
CustAcknowledge_s
#endif
;




#ifndef __cplusplus
typedef 
#endif
enum LnPrt_e
 {
  E_CURS_TO_END,
  E_CURS_TO_BEG,
  E_FULL_LINE
 }
#ifndef __cplusplus
LnPrt_e
#endif
;

#ifndef __cplusplus
typedef 
#endif
enum TimeForm_e
 {
  E_CAL_FORMAT,   /* Regular Calendar format d/m/y  h:m:s   */
  E_DBS_FORMAT,   /* Database fromat  yyyy/mm/dd - hh:mm:ss */
  E_DUR_FORMAT    /* Duration format  d-h:m:s               */
 }
#ifndef __cplusplus
TimeForm_e
#endif
;



/*======================================================================================================================*/

extern char const * const ResultColors[];   /* Colors for showing results : [0] - for incorrect result and [1] - for correct result. */

extern char const *LATSGN[];   // Latitude  sign "N" (North) in case of positive, "S" (South) in case of negative.
extern char const *LONGSGN[];  // Longitude sign "E" (East)  in case of positive, "W" (West)  in case of negative.


/*======================================================================================================================*/


double sqr(double x);

uint64_t RandGenLongLong();

/*======================================================================================================================*/


double GetDistance(GPS_Cords_s p1, GPS_Cords_s p2);

void CordsToString(char Buf[], int MaxSize, GPS_Cords_s GPSCords);

void PrintGPSCords(GPS_Cords_s CordsToPrint);

/*======================================================================================================================*/

void ConvertTime(time_t const * const TimeToConvert, char TimeAsStr[], size_t TimeStrSize, TimeForm_e TimeFormat);



/*======================================================================================================================*/

/**
 * @brief Calculates CRC from given block of data.
 *
 * @code
 * uint32_t FindCRC(uint8_t * Data, uint8_t Length, uint32_t InitVal);
 * @code
 *
 * @param Data     The pointer to the first address of memory in which the data exists.
 *
 * @param Length   The length of the data.
 *
 * @param InitVal The initialization value.
 *
 * @return Calculated CRC.
 *
 */
uint32_t FindCRC(uint8_t * Data, uint8_t Length, uint32_t InitVal);

/**
 * @brief Appends to the end of the data array the calculated CRC from it. The length must include the place of the CRC.
 *        For example if the data has length 8 the length given as parameter must be 12 = 8 + 4. The CRC has 4 bytes of length.
 *
 * @code
 * void Add_CRC(uint8_t buf[], size_t len);
 * @code
 *
 * @param buf  The start of data.
 *
 * @param len  The length of data including CRC size. For example if buffer has length of 8 bytes the len must be 12: 8 +4 = 12.
 */
void Add_CRC(uint8_t buf[], size_t len);

/**
 * @brief Checks if the CRC is correct.
 *
 * @code
 * bool CRC_Correct(uint8_t buf[], size_t len);
 * @code
 *
 * @param buf   The start of data
 *
 * @param len   The length of data. Including CRC.
 *              For example if the given length is 12 the CRC checking will be made from the first 8 bytes
 *              and the result will be compared to the last 4 bytes.
 *
 */
bool CRC_Correct(uint8_t buf[], size_t len);

/**
 * @brief
 * This function encodes the packet send to network.                                               
 * Attention !!!                                                                                   
 * The last parameter "NetSendData" is given as pointer to pointer to dynamically allocated memory.
 * That means that at the end of the program it must be freed by the procedure "FreeData()"        
 * to avoid the memory leakage.                                                                    
 * 
 * @code
 * ssize_t EncodeNetData(uint8_t const * const CustomData, uint8_t Len, uint8_t **NetSendData);
 * @code
 * 
 * @param CustomData   The pointer to the data to be encoded.
 * 
 * @param Len          Length of the data to be encoded (Length of the CustomData).
 * 
 * @param NetSendData  Encoded data.
 * 
 * @return             The length of data if encoded successfully or "0" if failed.
 * 
 */
ssize_t EncodeNetData(uint8_t const * const CustomData, uint8_t Len, uint8_t **NetSendData);


/**
 * @brief This function decodes the packet received from network.
 * 
 * @code
 * bool DecodeNetData(uint8_t NetRecData[], size_t Len, uint8_t *CustomData);
 * @code
 * 
 * @param NetRecData  Data for decoding.
 * 
 * @param CustomData  CustomData Data for decoding.
 * 
 * @param Len         Length of the data that must be after decoding (Length of the CustomData).
 * 
 * @return            "true" if the data was decoded successfully. "false" otherwise.
 */
bool DecodeNetData(uint8_t NetRecData[], size_t Len, uint8_t *CustomData);


/**
 * @brief
 * This Procedure is used for freeing the "**Data" reserved by one of procedure  "EncodeNetData()".
 * No need to check anything before running it because it checks automatically inside if the memory
 * is reserved and sets the pointer to NULL after freeing it.                                      
 * 
 * @code
 * void FreeData(uint8_t **Data);
 * @code
 * 
 * @param Data     The pointer to pointer to the dynamically allocated memory for freeing.
 * 
 */
void FreeData(uint8_t **Data);

/*======================================================================================================================*/

bool GetDataBaseFile(int const argc, char const *argv[], char NamePath[]);

bool GetPIDFile(int const argc, char const *argv[], char NamePath[]);

/*======================================================================================================================*/

/**
 * @brief Set cursor to defined place in the screen. 
 * 
 * @param x Number of the column.
 * 
 * @param y Number of the row.
 */
void MoveCursor(int x, int y);


/**
 * @brief Moves cursor forward.
 * 
 * @param x Number steps to move.
 */
void MoveCursFw(int x);

/**
 * @brief Moves cursor Up.
 * 
 * @param y Number of steps to move.
 */
void MoveCursUp(int y);


/**
 * @brief Moves cursor backward.
 * 
 * @param x Number of steps to move.
 */
void MoveCursBw(int x);

/**
 * @brief Moves cursor down.
 * 
 * @param Number of steps to move.
 */
void MoveCursDn(int y);

/**
 * @brief Sets cursor place in the line were it exists.
 * 
 * @param col x-position in line where the cursor must be put.
 */
void MoveCursToCol(int col);

/**
 * @brief Clears line in console.
 * 
 * @param lp The type of cleaning: after cursor, before cursor or all the line from beginning to end.
 */
void ClearLine(LnPrt_e lp);


/**
 * @brief Prints horizontal scale bar in the cursor place
 * 
 * @param ScaleLength Total length of the scale including number. (A part of the length will be taken by the number.)
 * 
 * @param FilledLen   The part of the scale-bar that is filled.
 * 
 * @param Colors      The filled part of the bar and empty part of the bar colors.
 * 
 * @param Symbols     The symbols from which is built a filled and the empty parts
 * 
 * @param MaxValue    The biggest number shown in the scale.
 * 
 * @param Value       The value shown in the scale.
 * 
 * @param Units       The units that are shown in the scale right after value.
 */
void PrintHorizScale(uint32_t ScaleLength, uint32_t FilledLen, char *Colors[], char *Symbols[], uint32_t MaxValue, uint32_t Value, char Units[]);

/*======================================================================================================================*/




#ifdef __cplusplus
}
#endif

#endif //  ____CommonData_h__
