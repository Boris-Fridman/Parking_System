
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

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(__linux__)
//#if !(defined(__GNUC__) || defined(__ICCARM__) || defined(__CC_ARM) )
#include <netdb.h>
#else
#include <time.h>
#endif



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


#define PROC_NAME_COLOR         TermBrightCyan
#define PROC_PID_COLOR          TermBrightMagenta
#define CITYNAME_COLOR          TermYello
#define PRICE_COLOR             TermBlue  //  TermMagenta
#define PRICEUNITS_COLOR        TermMagenta // TermBrightMagenta

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



#define DEF_CLIENT_NAME           "Boris Fridman"
#define DEF_VEHICLE_ID            13248551

//#define DESTIN_IP               "127.0.0.1"    // For test only. In the future will be removed.
#define DESTIN_IP                 "192.168.1.164"            /* Server IP address to which are sent the call messages. */
#define DESTIN_PORT                8080                      /* Server port to which are sent the GPS messages. */
#define BUFFER_SIZE                1024                      /* The length in bytes, of the buffer pointed by the buf parameter that is used by the recv() function. */

#define CRC_SIZE                  ( sizeof(uint32_t) )



#define NAME_LEN                  30
#define PATH_LEN                  300
#define LOGMSG_LEN                350

#define DB_FILENAME               "ParkingInfo.sqlite3"
#define CONF_DIR_NAME             "ConfDir"
#define LOG_FILENAME              "ParkingInfo.log"
#define LOG_CAR_FILENAME          "CarParkingInfo.log"
#define DB_MAN_PID_FILENAME       "DB_MAN_PID"
#define DB_UPADATE_SIGNAL         SIGUSR1

#define SHP_FILENAME              "Regions/Regions.shp"         /* The name of the shape file having the information about the cities regions. In the same path must be included the files with extentions ".dbf" and ".shx" containing the same name */
#define SHP_FIELD_NAME            "latin name"
#define SHP_OSM_ID                "osm_id"

#define I2C_ADDR                  0x55

#define DEF_MAX_PARK_TIME         1800                          /* Maximum time for staying on a parking in seconds. The time will be selected randomly between 0 upto defined value. Giving zero value will mean infinit.                                                              */
#define DEF_MAX_PARK_DELAY        40                            /* Maximum time for waiting between parkings in seconds. The real value will be generated between 0 upto the given value.                                                                                               */

#define MIN_PARK_TIME             60                            /* Minimum time for staying on a parking in seconds that can be generated. Attention !!! This value must be bigger than zero otherwise if the random-generated value is set to zero the program will not repark at all. */
#define MIN_PARK_WAIT_TIME        60                            /* Minimum time for waiting between parkings in seconds that can be generated. Should not be set to zero because it wasn't tested with it.                                                                              */

/*======================================================================================================================*/

/**
 * Macro for preventing warnings in case of unused variables.
 */
#if !defined(UNUSED)
#define UNUSED(X)                 (void)X      /* To avoid gcc/g++ warnings */
#endif /* UNUSED */

#define STRINGIZE_DETAIL(x) #x
#define TO_STRING(x) STRINGIZE_DETAIL(x)

/*======================================================================================================================*/

/*----------------------------------------------------------------------------------------------------------------------*/
/* The structure for defining the screen message type. Is requred for deciding in which color to show a message.        */
#ifndef __cplusplus
typedef 
#endif
enum Error_Results_e
 {
  E_FAIL,
  E_CORRECT,
  E_ATTENTION,
  E_WARNING,
  E_SUCCESS,
  E_PROBLEM
 }
 #ifndef __cplusplus
 Error_Results_e
 #endif
 ;

/*----------------------------------------------------------------------------------------------------------------------*/
/* The queue direction enumeration defines if the messages via a queue can be sent, received or sent and received.      */
#ifndef __cplusplus
typedef 
#endif
enum QueueDirection_e
 {
  QUEUE_SEND_E,
  QUEUE_RECEIVE_E,
  QUEUE_SEND_RECEIVE_E
 }
#ifndef __cplusplus
QueueDirection_e
#endif
;

/*----------------------------------------------------------------------------------------------------------------------*/
/* Defines the point state in reference the city-defining polygon outisde, inside or exactly on a bound.                */
#ifndef __cplusplus
typedef 
#endif
enum PointState_e
 {
  OUTSIZE_E,
  ONBOUND_E,
  INSIDE_E
 }
#ifndef __cplusplus
PointState_e
#endif
;

/*----------------------------------------------------------------------------------------------------------------------*/
/* GPS coordinates defininig structure longitude and latitude. The coordinates are given in degrees.                    */
/* GPS Coordinates. Are given in degrees. */
#ifndef __cplusplus
typedef 
#endif
struct GPS_Cords_s
 {
  double Longitude;   /* (˚) (deg) */  /* Positive number - East  , Negative number - West  .     > 0 - EL ;  < 0 - WL .  */
  double Latitude;    /* (˚) (deg) */  /* Positive number - North , Negative number - South .     > 0 - NL ;  < 0 - SL .  */
 }
 #ifndef __cplusplus
 GPS_Cords_s
 #endif
 ;

/*----------------------------------------------------------------------------------------------------------------------*/
/* Strucutre for defining the distance coordinates that are given in kilometers. Are used for calculating a distance.   */
#ifndef __cplusplus
typedef 
#endif
struct Dist_Cords_s
 {
  double x; /* Longitude; */ /* km */ /* Positive number - East  , Negative number - West  .  */
  double y; /* Latitude;  */ /* km */ /* Positive number - North , Negative number - South .  */
 }
 #ifndef __cplusplus
 Dist_Cords_s
 #endif
 ;

/*----------------------------------------------------------------------------------------------------------------------*/
/* The structure for defining the 3D-Spce coordinates that are given in kilometers.                                     */
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

/*----------------------------------------------------------------------------------------------------------------------*/
//  #ifndef __cplusplus
//  typedef 
//  #endif
//  struct Parking_s
//   {
//    GPS_Cords_s Cords;
//    uint16_t    Parking_ID;
//    uint16_t    MaxNumPlaces;
//    uint16_t    NumPlaces;
//    uint16_t    Price;              /*  0.01₪ / hour  */
//    char Name[NAME_LEN];
//   }
// #ifndef __cplusplus  
// Parking_s
// #endif
// ;

/*----------------------------------------------------------------------------------------------------------------------*/
/* Parking price structure used for sending the parking information via the queue.                                      */
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

/*----------------------------------------------------------------------------------------------------------------------*/
/* Client information structure, Is used for sending data via the network from client to srver.     Client ▬▬▬▶ Server  */
 #ifndef __cplusplus
 typedef 
 #endif
 struct Customer_s
  {
   GPS_Cords_s Cords;
   uint32_t    Vechicle_ID;
   char        Customer_Name[NAME_LEN];
  }
#ifndef __cplusplus  
Customer_s
#endif
;

/*----------------------------------------------------------------------------------------------------------------------*/
/* Server response information strucutre. Is used for giving response acknowledge back.               Clent ◀▬▬▬ Server */
#ifndef __cplusplus
typedef 
#endif
struct CustAcknowledge_s
 {
  uint32_t Vechicle_ID;
  uint32_t OSM_ID;
  uint16_t City_ID;
  time_t   ParkingStartTime;
  time_t   ParkingEndTime;
  time_t   ParkingDurationTime;           /* seconds    */
  uint16_t AccumulatedPrice;              /*  0.01₪     */
  uint16_t PricePerHour;                  /*  0.01₪ / h */
  char     City_Name[NAME_LEN];
 }
#ifndef __cplusplus  
CustAcknowledge_s
#endif
;

/*----------------------------------------------------------------------------------------------------------------------*/
/* Structure for sending real-time parking information via queue to DataBase.                                           */
#ifndef __cplusplus
typedef 
#endif
struct ClientQueueMsg_s
 {
  GPS_Cords_s Cords;
  uint32_t    Vechicle_ID;
  uint32_t    OSM_ID;
  uint16_t    City_ID;
  time_t      ParkingStartTime;
  time_t      ParkingEndTime;
  time_t      ParkingDurationTime;           /* seconds       */
  uint16_t    PricePerHour;                  /*  0.01₪ / hour */
  uint16_t    AccumulatedPrice;              /*  0.01₪        */
  char        Customer_Name[NAME_LEN];
  char        City_Name[NAME_LEN];
 }
#ifndef __cplusplus
ClientQueueMsg_s 
#endif
;

/*----------------------------------------------------------------------------------------------------------------------*/
/* Parking information structure for sending the information about parking from GPS program vis I2C. GPSMan ▬▬▬▶ CarMan */
#ifndef __cplusplus
typedef
#endif
struct ParkingData_s
 {
  char ParkingName[64];  /* The name of the parking must be at least 64 chars because the most longest name is very big (has about 62 characters). */
  GPS_Cords_s ParkingCords;
 }
#ifndef __cplusplus
ParkingData_s
#endif
;

/*----------------------------------------------------------------------------------------------------------------------*/
/* Cursor Position structure. Is defined for screen clearing procedure.                                                 */
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

/*----------------------------------------------------------------------------------------------------------------------*/
/* Printing / Showing date and time format type enumeration.                                                            */
#ifndef __cplusplus
typedef 
#endif
enum TimeForm_e
 {
  E_CAL_FORMAT,   /* Regular Calendar format d/m/y  h:m:s   */
  E_DBS_FORMAT,   /* Database format  yyyy/mm/dd - hh:mm:ss */
  E_DUR_FORMAT    /* Duration format  d-h:m:s               */
 }
#ifndef __cplusplus
TimeForm_e
#endif
;

/*----------------------------------------------------------------------------------------------------------------------*/
/* Printing / Showing price format units' type enumeration.                                                             */
#ifndef __cplusplus
typedef 
#endif
enum PriceFormat_e
 {
  E_PPH_FULL_FORMAT,   /* Price Per Hour format     - ₪/hour */
  E_PPH_FORMAT,        /* Price Per Hour format     - ₪/h    */
  E_ACC_FORMAT,        /* Accumulated price format  - ₪      */

  NUM_PRICE_VARIANTS
 }
#ifndef __cplusplus
PriceFormat_e
#endif
;



/*======================================================================================================================*/

extern char const * const ResultColors[];   /* Colors for showing results : [0] - for incorrect result and [1] - for correct result. */

extern char const *LATSGN[];   // Latitude  sign "N" (North) in case of positive, "S" (South) in case of negative.
extern char const *LONGSGN[];  // Longitude sign "E" (East)  in case of positive, "W" (West)  in case of negative.


/*======================================================================================================================*/

/**
 * @brief Raises numbers to the square power.
 * 
 * @code
 * double sqr(double x);
 * @code
 * 
 * @param x The number from which is calculated the square power
 * 
 * @return The given parameter multiplied by itself. x²
 * 
 */
double sqr(double x);

/**
 * @brief Generates random numbers in 64 bit size.
 * 
 * @code 
 * uint64_t RandGenLongLong();
 * @code
 * 
 * @return The random generated number in 64-bit length.
 */
uint64_t RandGenLongLong();

/**
 * @brief Generates random numbers between MinNumber and MaxNumber including.
 * 
 * @code
 * uint32_t GenRandNumber(uint32_t MinNumber, uint32_t MaxNumber);
 * @code
 * 
 * @param MinNumber Minimal Number that can be generated.
 * 
 * @param MaxNumber Maximal Number that can be generated.
 * 
 * @return Random generated Number between the given limits including them.
 */
uint32_t GenRandNumber(uint32_t MinNumber, uint32_t MaxNumber);

/**
 * @brief Finds log on decimal base without using real numbers with rounding up or down according second parameter.
 * 
 * @code
 * int32_t IntLog10(uint32_t Value, uint8_t RoundDirrection);
 * @code
 * 
 * @param Value           The value from which the logarithm is calculated.
 * 
 * @param RoundDirrection "0" if the result must be rounded down or "1" if must be rounded up.
 * 
 * @return Log10 from value rounded up or down according to the "RoundDirrection" Flag.
 */
int32_t IntLog10(uint32_t Value, uint8_t RoundDirrection);

/*======================================================================================================================*/

/**
 * @brief Checks if the given GPS Point is inside the givne zone polygon. 
 * 
 * @code
 * PointState_e PointInPoly(const GPS_Cords_s BoundingPoly[], size_t PolySize, GPS_Cords_s Cords);
 * @code
 * 
 * @param BoundingPoly The polygon defining any city or village zone.
 * 
 * @param PolySize     The number of points that the bounding polygon contains.
 * 
 * @param Cords        The GPS coordinates of the point been checke.
 * 
 * @return             Result if the point is in the zone of the polygon, out of it or on the bound itself.
 */
PointState_e PointInPoly(const GPS_Cords_s BoundingPoly[], size_t PolySize, GPS_Cords_s Cords);

/**
 * @brief Finds the distance from the given any two points on the Earth. 
 *        The function calculates distance by taking in account the middle radius/diameter of the Earth.
 *        The function doesn't nake in account any relief.
 * 
 * @code 
 * double GetDistance(GPS_Cords_s p1, GPS_Cords_s p2);
 * @code
 * 
 * @param p1 The first  one of the two GPS points between which must be calculated the distance.
 * 
 * @param p2 The second one of the two GPS points between which must be calculated the distance.
 * 
 * @return The distance between the two points "p1" and "p2".
 */
double GetDistance(GPS_Cords_s p1, GPS_Cords_s p2);

/**
 * @brief Converts the given GPS coordinates and creates a string showing them in degrees, minutes, secons and decimals of seconds. 
 *        In addition adds to them the tags which of them is the longitude and which one is the latitude.
 *        The south latitude and the west longitude will be shown with the negative sign. 
 *        The north latitude and the east longitude will be shown without negative sign.
 * 
 * @code
 * void CordsToString(char Buf[], int MaxSize, GPS_Cords_s GPSCords);
 * @code
 * 
 * @param Buf      The string buffer to which is put the GPS information. The size must be at least 40 characters.
 * 
 * @param MaxSize  The maximum length of the buffer to which is copied the information. 
 *                 The information must be at least 40 characters length.
 *                 If the length is not big enough the part of the information will be lost.
 * 
 * @param GPSCords The coordinates that are given for putting them to the string.
 */
void CordsToString(char Buf[], int MaxSize, GPS_Cords_s GPSCords);

/**
 * @brief Converts the given GPS coordinates and creates a string showing them in degrees, minutes, secons and decimals of seconds. 
 *        In addition adds to them the tags which of them is the longitude and which one is the latitude.
 *        The south latitude and the west longitude will be shown with the negative sign. 
 *        The north latitude and the east longitude will be shown without negative sign.
 *        The coordinates can be colored or not depending of requirements.
 *        In case of coloring the bufer must be at least of 200 characters length. Without coloring only 50.
 * 
 * @code
 * void CreateCordsFormatted(char Buf[], int MaxSize, GPS_Cords_s GPSCords, bool Colored);
 * @code
 * 
 * @param Buf      The string buffer to which is put the GPS information. The size must be at least 40 characters.
 * 
 * @param MaxSize  The maximum length of the buffer to which is copied the information. 
 *                 The information must be at least 200 characters length in coloring mode or 50 characterss if not.
 *                 If the length is not big enough the part of the information will be lost.
 * 
 * @param GPSCords The coordinates that are given for putting them to the string.
 * 
 * @param Colored  If set "true" if the coordinates must be colorde or "false" if not. 
 *                 Is useful if the printf is redirected to any output file.
 */
void CreateCordsFormatted(char Buf[], int MaxSize, GPS_Cords_s GPSCords, bool Colored);

/**
 * @brief Prints the GPS coordinates romatted byt the "CordsToString()" procedure.
 * 
 * @code
 * void PrintGPSCords(GPS_Cords_s CordsToPrint);
 * @code
 * 
 * @param CordsToPrint The coordinates that are given for printing.
 */
void PrintGPSCords(GPS_Cords_s CordsToPrint);

/*======================================================================================================================*/

/**
 * @brief Converts the Vehicle ID number to divided one with lines depending of its length as in the israelian stadards.
 *        For example: the number with 6 digits will be converted to xxx-xxx.
 *                                with 7 digits will be converted to xx-xxx-xx.
 *                                with 8 digits will be converted to xxx-xx-xxx.
 * 
 * @code
 * void VehicleIDToString(char Buf[], int MaxSize, uint32_t VehcleID);
 * @code
 * 
 * @param Buf      The output string buffer to which is written the ID number.
 * 
 * @param MaxSize  The buffer size. If the buffer is smaller than required the part of the data will be lost. 
 * 
 * @param VehcleID The Vehicle ID Number.
 */
void VehicleIDToString(char Buf[], int MaxSize, uint32_t VehcleID);

/**
 * @brief Converts the Vehicle ID number to divided one with lines depending of its length as in the israelian stadards.
 *        For example: the number with 6 digits will be converted to xxx-xxx.
 *                                with 7 digits will be converted to xx-xxx-xx.
 *                                with 8 digits will be converted to xxx-xx-xxx.
 *        The number finaly will be formatted and colored as israelian standarted number if the coloring is enabled.
 * 
 * @code
 * void CreateVehIDFormated(char Buf[], int MaxSize, uint32_t VehcleID, bool Colored);
 * @code
 * 
 * @param Buf      The output string buffer to which is written the ID number.
 * 
 * @param MaxSize  The buffer size. If the buffer is smaller than required the part of the data will be lost. 
 * 
 * @param VehcleID The Vehicle ID Number.
 * 
 * @param Colored  "true" if coloring must be enabled or "false" if not.
 */
void CreateVehIDFormated(char Buf[], int MaxSize, uint32_t VehcleID, bool Colored);

/*======================================================================================================================*/

/**
 * @brief Creates the string with formatted name of a client that can be then shown on a screen through a terminal.
 * 
 * @code
 * void CreateNameFormated(char Buf[], int MaxSize, char const Name[], bool Colored);
 * @code
 * 
 * @param Buf     The output buffer to which the string with the formatted name is written.
 * 
 * @param MaxSize The size of the buffer. If the buffer is too small the part of data will be lost.
 * 
 * @param Name    The name of the client to be formatted.
 * 
 * @param Colored "true" if the formatted name must be colored or "false" if not.
 */
void CreateNameFormated(char Buf[], int MaxSize, char const Name[], bool Colored);

/*======================================================================================================================*/

/**
 * @brief Converts time to string format.
 * 
 * @code
 * void ConvertTime(time_t const * const TimeToConvert, char TimeAsStr[], size_t TimeStrSize, TimeForm_e TimeFormat);
 * @code
 * 
 * @param TimeToConvert Time data to be converted.
 * 
 * @param TimeAsStr     The given string buffer to which is put the given time converted to string.
 * 
 * @param TimeStrSize   The given string buffer's size. If the buffer is smaller than needed a part of data will be lost.
 * 
 * @param TimeFormat    The time format to which the time is converted. 
 */
void ConvertTime(time_t const * const TimeToConvert, char TimeAsStr[], size_t TimeStrSize, TimeForm_e TimeFormat);

/*======================================================================================================================*/

/**
 * @brief Converts price to string format.
 * 
 * @code
 * void ConvertPrice(uint16_t PriceToConvert, char PriceAsString[], size_t PriceStrgSize, PriceFormat_e PriceFormat, bool Formated);
 * @code
 * 
 * @param PriceToConvert The price to be converted.
 * 
 * @param PriceAsString  The output string buffer to which is put the formatted price.
 * 
 * @param PriceStrgSize  The output string buffer's size. If the size is too small a part of data will be lost.
 */
void ConvertPrice(uint16_t PriceToConvert, char PriceAsString[], size_t PriceStrgSize, PriceFormat_e PriceFormat, bool Formated);

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
 * @param InitVal  The initialization value.
 *
 * @return Calculated CRC.
 */
uint32_t FindCRC(uint8_t const *Data, uint8_t Length, uint32_t InitVal);

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
 */
bool CRC_Correct(uint8_t const buf[], size_t len);

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
bool DecodeNetData(uint8_t const NetRecData[], size_t Len, uint8_t *CustomData);


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
 */
void FreeData(uint8_t **Data);

/*======================================================================================================================*/

/**
 * @brief Checks if the path is local and if it is - converts it to full according the path of this executed program.
 * 
 * @code
 * void AdjustPath(char const *OwnProgName, char *PathToAdjust, size_t const MaxSize);
 * @code
 * 
 * @param OwnProgPathName The name with path this running program itself.
 * 
 * @param PathToAdjust    The path that is adjusted.
 * 
 * @param MaxSize      The maximum lingth of the the returned string. 
 *                     If the length is smaller than required the part of the data will be lost.
 */
void AdjustPath(char const *OwnProgPathName, char *PathToAdjust, size_t const MaxSize);

/**
 * @brief Generates a name of a configuration .ini file according to the name of this running executable file.
 *        The extention in this case will be removed. For example:  "program"       ▬▬▬▶   "program.ini".
 *                                                                  "program.elf"   ▬▬▬▶   "program.ini"
 * @code
 * void GetConfFileName(char const *OwnProgPathName, char *ConfFileName, size_t const MaxSize);
 * @code
 * 
 * @param OwnProgPathName The name with path this running program itself.
 * 
 * @param ConfFileName    The returned name of the configuration file.
 * 
 * @param MaxSize         The maximum lingth of the the returned string. 
 *                        If the length is smaller than required the part of the data will be lost.
 */
void GetConfFileName(char const *OwnProgPathName, char *ConfFileName, size_t const MaxSize);

/**
 * @brief Gives the path of the folder in which can be stored the configuration files. 
 * 
 * @code 
 * bool GetConfigPath(char const *OwnProgPathName, char NamePath[], size_t const MaxSize);
 * @code
 * 
 * @param OwnProgPathName The name with path this running program itself.
 * 
 * @param NamePath        The returned path to the configuration file.
 * 
 * @param MaxSize         The maximum lingth of the the returned string. 
 *                        If the length is smaller than required the part of the data will be lost.
 * 
 * @return                "true" if the folder was created. Otherwise returns "false".
 */
bool GetConfigPath(char const *OwnProgPathName, char NamePath[], size_t const MaxSize);

/**
 * @brief Gives the name with path of the database file. 
 * 
 * @code
 * bool GetDataBaseFile(char const *OwnProgPathName, char NamePath[], size_t const MaxSize);
 * @code
 * 
 * @param OwnProgPathName The name with path this running program itself.
 * 
 * @param NamePath        The returned name of the database file including path to it.
 * 
 * @param MaxSize         The maximum lingth of the the returned string. 
 *                        If the length is smaller than required the part of the data will be lost.
 * 
 * @return "true" if the folder was created. Otherwise returns "false".
 */
bool GetDataBaseFile(char const *OwnProgPathName, char NamePath[], size_t const MaxSize);

/**
 * @brief Gives the name with path of the file with the program PID.
 * 
 * @code 
 * bool GetPIDFile(char const *OwnProgPathName, char NamePath[], size_t const MaxSize);
 * @code
 * 
 * @param OwnProgPathName The name with path this running program itself.
 * 
 * @param NamePath The returned name of the containing PIDFile including its path.
 * 
 * @param MaxSize         The maximum lingth of the the returned string. 
 *                        If the length is smaller than required the part of the data will be lost.
 * 
 * @return "true" if the folder was created. Otherwise returns "false". *   
 */
bool GetPIDFile(char const *OwnProgPathName, char NamePath[], size_t const MaxSize);

/**
 * @brief  Gives the name with path of the file containing the map-shapes of cities, towns, vilages and other geographic places.
 * 
 * @code
 * bool GetShapeFile(char const *OwnProgPathName, char NamePath[], size_t const MaxSize);
 * @code
 * 
 * @param OwnProgPathName The name with path this running program itself.
 * 
 * @param NamePath        The returned name of the map-shapes file including its path.
 * 
 * @param MaxSize         The maximum lingth of the the returned string. 
 *                        If the length is smaller than required the part of the data will be lost.
 * 
 * @return "true" if the folder was created. Otherwise returns "false".
 */
bool GetShapeFile(char const *OwnProgPathName, char NamePath[], size_t const MaxSize);

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(__linux__)  /* For Linux */

/**
 * @brief Checks if the copy of the current program is allready running. Returns "true" if running or "false" if not.
 * 
 * @code
 * bool PrevProcCopyRunning(char const *OwnProgPathName, bool ByNameAndPath);
 * @code
 * 
 * @param OwnProgPathName The name with path this running program itself.
 * 
 * @param ByNameAndPath   Set to "false" if additional copy is not permitted at all or "true" if only from the same path.
 * 
 * @return "true" if the one copy of the current program is allready runnig or "false" if not.
 */
bool PrevProcCopyRunning(char const *OwnProgPathName, bool ByNameAndPath);

/**
 * @brief Gives the name with the full path of the current running program by checking the /proc/<PID>/exe link file.
 * 
 * @code
 * void GetOwnNamePath(char OwnPathToRet[], size_t const MaxSize);
 * @code
 * 
 * @param OwnPathToRet The returned name of this current running program including its path.
 * 
 * @param MaxSize      The maximum lingth of the the returned string. 
 *                     If the length is smaller than required the part of the data will be lost.
 */
void GetOwnNamePath(char OwnPathToRet[], size_t const MaxSize);

#endif

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
