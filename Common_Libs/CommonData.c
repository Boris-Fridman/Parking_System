
#include "CommonData.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifdef __cplusplus
#include <cstring>
#include <cmath>
#else
#include <string.h>
#include <math.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>

#include <libgen.h>
#include <time.h>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(__linux__)  // For Linux
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#else  // For ARM
#endif


/*======================================================================================================================*/

char const *const ResultColors[] = {TermRed, TermGreen, TermBrightYello, TermYello, TermCyan, TermMagenta};

/*======================================================================================================================*/

#define EARTH_DIAMETER 12742.018 /* km */               /* Mean earth diameter       */
#define EARTH_RADIUS (EARTH_DIAMETER / 2)      /* km */ /* Mean earth radius         */

#define EARTH_DIAMETER_P 12713.505             /* km */ /* Polar earth diameter      */
#define EARTH_DIAMETER_E 12756.274             /* km */ /* Equatorial earth diameter */

#define EARTH_RADIUS_P (EARTH_DIAMETER_P / 2) /* km */ /* Polar earth radius         */
#define EARTH_RADIUS_E (EARTH_DIAMETER_E / 2) /* km */ /* Equatorial earth radius    */

char const *LATSGN[]  = {"N", "S"}; // Latitude  sign "N" (North) in case of positive, "S" (South) in case of negative.
char const *LONGSGN[] = {"E", "W"}; // Longitude sign "E" (East)  in case of positive, "W" (West)  in case of negative.

/*======================================================================================================================*/
/*
 * *************************************************************************************************************
 **          Additional mathematics Functions.
 * *************************************************************************************************************
 */

/*----------------------------------------------------------------------------------------------------------------------*/
/* Raises numbers to the square power.                                                                                  */
double sqr(double x)
 {
  return x * x;
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Generates random numbers in 64 bit size.                                                                             */
uint64_t RandGenLongLong()
 {
  return ((uint64_t)rand() << 32) | ((uint64_t)rand());
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Generates random numbers between MinNumber and MaxNumber including.                                                  */
uint32_t GenRandNumber(uint32_t MinNumber, uint32_t MaxNumber)
 {
  return MinNumber + rand() % (MAX(MaxNumber, MinNumber) - MinNumber + 1);
 }


/*----------------------------------------------------------------------------------------------------------------------*/
/* Finds log on decimal base without using real numbers with rounding up or down according second parameter.            */
int32_t IntLog10(uint32_t Value, uint8_t RoundDirrection) /*   "RoundDirrection=0" rounds down "RoundDirrection"=1 rounds up */
 {
  int32_t Result;
  /*  If rounding up is required the comparison result will be "(Value!=0)". If required the rounding down the comparison result will be "(Value>=10)".   */
  for (Result = 0; (RoundDirrection ? (Value != 0) : (Value >= 10)); Value /= 10)
   {
    ++Result;
   }
  return Result;
 }

/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Coordinates and distance Functions.
 * *************************************************************************************************************
 */

#define PERMITED_ERROR 0.01


#ifndef __cplusplus
typedef
#endif
struct AngDegMinSec_s
 {
  int16_t Deg;
  uint8_t Min;
  uint8_t Sec;
  double Dec;
 }
#ifndef __cplusplus
AngDegMinSec_s
#endif
;

/*----------------------------------------------------------------------------------------------------------------------*/
/* Checks if the given GPS Point is inside the givne zone polygon.                                                      */
PointState_e PointInPoly(const GPS_Cords_s BoundingPoly[], size_t PolySize, GPS_Cords_s Cords)
 {
  size_t i;
  double x1, x2, y1, y2, yt, xt;
  double angle, dang;
  bool inside, outside;
  size_t sz;
  sz = PolySize;
  for(i = 0, angle = 0; i < PolySize; i++)
   {
    x1 = BoundingPoly[ i          ].Longitude - Cords.Longitude;
    y1 = BoundingPoly[ i          ].Latitude  - Cords.Latitude ;
    x2 = BoundingPoly[(i + 1) % sz].Longitude - Cords.Longitude;
    y2 = BoundingPoly[(i + 1) % sz].Latitude  - Cords.Latitude ;

    xt = x1 * x2 + y1 * y2;
    yt = x1 * y2 - x2 * y1;

    if((xt == 0) && (yt == 0))  /* Checking if the tested point is on any bounding point limit. if this happens arctg(yt/xt) means arctg(0/0) that isn't defined. In addition it means that the point is placed on the bounding corner. */
     return ONBOUND_E;  /* The tested point exists on the corner of the bounding polynom. Nothing to check else. */

    dang = atan2(yt, xt);

    if(fabs(dang) == M_PI) /* Checking if the angle is equivalent to 180° or π radians that means that the point exist exactely on the bounding line. */
     return ONBOUND_E;  /* The point exists on the line of the bounding polynom. Nothing to check else. */
    
    angle += dang;
   }

  angle = fabs(angle);  // In case of the oposite rotation the angle will be -2π instead of 2π.
  outside = fabs(angle             ) < PERMITED_ERROR;  //  Outsize
  inside  = fabs(angle - (2 * M_PI)) < PERMITED_ERROR;  //  Insize
  if(outside) return OUTSIZE_E;
  else if(inside) return INSIDE_E;
  else return ONBOUND_E;
 }



void AngToDegMinSecDec(double Angle, AngDegMinSec_s *ConvAng)
 {
  bool sgn = Angle < 0;
  Angle = fabs(Angle);
  ConvAng->Deg = floor(Angle);
  ConvAng->Min = floor((Angle - ConvAng->Deg) * 60);
  ConvAng->Sec = floor(((Angle - ConvAng->Deg) * 60 - ConvAng->Min) * 60);
  ConvAng->Dec = ((Angle - ConvAng->Deg) * 60 - ConvAng->Min) * 60 - ConvAng->Sec;
  ConvAng->Deg *= (sgn ? -1 : 1);
 }

void GPSToSpace(Space_Cords_s *SpaceCords, GPS_Cords_s GPS_Data)
 {
  SpaceCords->x = EARTH_RADIUS_E * cos(GPS_Data.Latitude * M_PI / 180) * cos(GPS_Data.Longitude * M_PI / 180);
  SpaceCords->y = EARTH_RADIUS_E * cos(GPS_Data.Latitude * M_PI / 180) * sin(GPS_Data.Longitude * M_PI / 180);
  SpaceCords->z = EARTH_RADIUS_P * sin(GPS_Data.Latitude * M_PI / 180);
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Finds the distance from the given any two points on the Earth.                                                       */
/* The function calculates distance by taking in account the middle radius/diameter of the Earth.                       */
/* The function doesn't nake in account any relief.                                                                     */
double GetDistance(GPS_Cords_s p1, GPS_Cords_s p2)
 {
  Space_Cords_s s1, s2;
  double LinDist, ArcDist;
  GPSToSpace(&s1, p1);
  GPSToSpace(&s2, p2);
  LinDist = sqrt(sqr(s1.x - s2.x) + sqr(s1.y - s2.y) + sqr(s1.z - s2.z));
  ArcDist = asin(LinDist / EARTH_RADIUS / 2) * EARTH_RADIUS * 2;
  return ArcDist;
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Converts the given GPS coordinates and creates a string showing them in degrees, minutes, secons and second decimals.*/
/* In addition adds to them the tags which of them is the longitude and which one is the latitude.                      */
/* The south latitude and the west longitude will be shown with the negative sign.                                      */
/* The north latitude and the east longitude will be shown without negative sign.                                       */
void CordsToString(char Buf[], int MaxSize, GPS_Cords_s GPSCords)
 {
  AngDegMinSec_s LatConvAng, LongConvAng;
  AngToDegMinSecDec(GPSCords.Latitude, &LatConvAng);
  AngToDegMinSecDec(GPSCords.Longitude, &LongConvAng);
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(__linux__)  // For Linux
  snprintf(Buf, MaxSize - 1, "%d˚%d'%d\".%03d LONG %d˚%d'%d\".%03d LAT", LongConvAng.Deg, LongConvAng.Min, LongConvAng.Sec, (int32_t)round(LongConvAng.Dec * 1000), LatConvAng.Deg, LatConvAng.Min, LatConvAng.Sec, (int32_t)round(LatConvAng.Dec * 1000));
#else  // For ARM
  snprintf(Buf, MaxSize - 1, "%d˚%d'%d\".%03ld LONG %d˚%d'%d\".%03ld LAT", LongConvAng.Deg, LongConvAng.Min, LongConvAng.Sec, (int32_t)round(LongConvAng.Dec * 1000), LatConvAng.Deg, LatConvAng.Min, LatConvAng.Sec, (int32_t)round(LatConvAng.Dec * 1000));
#endif
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Converts the given GPS coordinates and creates a string showing them in degrees, minutes, secons and second decimals.*/
/* In addition adds to them the tags which of them is the longitude and which one is the latitude.                      */
/* The south latitude and the west longitude will be shown with the negative sign.                                      */
/* The north latitude and the east longitude will be shown without negative sign.                                       */
/* The coordinates can be colored or not depending of requirements.                                                     */
/* In case of coloring the bufer must be at least of 200 characters length. Without coloring only 50.                   */
void CreateCordsFormatted(char Buf[], int MaxSize, GPS_Cords_s GPSCords, bool Colored)
 {
  int l;
  char const *uc = (Colored ? TermBrightYello   : ""); /* Units       Color */
  char const *nc = (Colored ? TermBrightCyan    : ""); /* Numbers     Color */
  char const *pc = (Colored ? TermBrightMagenta : ""); /* Points      Color */
  char const *dc = (Colored ? TermBrightGreen   : ""); /* Description Color */
  char const *rc = (Colored ? TermColorsReset   : ""); /* Reset       Color */
  AngDegMinSec_s LatConvAng, LongConvAng;
  AngToDegMinSecDec(GPSCords.Latitude, &LatConvAng);
  AngToDegMinSecDec(GPSCords.Longitude, &LongConvAng);
  
  //  🌍🌎🌏🌐📍🗺🧭

  snprintf(Buf, MaxSize, "%s🌐 ",  (Colored ? TermBGBlack : ""));
  l = strlen(Buf);

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(__linux__)  // For Linux
  snprintf(&Buf[l], MaxSize - l, "%s%d%s˚%s%d%s'%s%d%s\"%s.%s%03d %sLONG %s%d%s˚%s%d%s'%s%d%s\"%s.%s%03d%s LAT", nc, LongConvAng.Deg, uc, nc, LongConvAng.Min, uc, nc, LongConvAng.Sec, uc, pc, nc, (int32_t)round(LongConvAng.Dec * 1000), dc, nc, LatConvAng.Deg, uc, nc, LatConvAng.Min, uc, nc, LatConvAng.Sec, uc, pc, nc, (int32_t)round(LatConvAng.Dec * 1000), dc);
#else  // For ARM
  snprintf(&Buf[l], MaxSize - l, "%s%d%s˚%s%d%s'%s%d%s\"%s.%s%03ld %sLONG %s%d%s˚%s%d%s'%s%d%s\"%s.%s%03ld%s LAT", nc, LongConvAng.Deg, uc, nc, LongConvAng.Min, uc, nc, LongConvAng.Sec, uc, pc, nc, (int32_t)round(LongConvAng.Dec * 1000), dc, nc, LatConvAng.Deg, uc, nc, LatConvAng.Min, uc, nc, LatConvAng.Sec, uc, pc, nc, (int32_t)round(LatConvAng.Dec * 1000), dc);
#endif

  //CordsToString(&Buf[l], MaxSize - l, GPSCords);
  l = strlen(Buf);
  snprintf(&Buf[l], MaxSize - l, "%s", rc);  
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Prints the GPS coordinates romatted byt the "CordsToString()" procedure.                                             */
void PrintGPSCords(GPS_Cords_s CordsToPrint)
 {
  char buf[200];
  CordsToString(buf, sizeof(buf), CordsToPrint);
  printf("%s", buf);
 }

/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Vehicle ID formating Procedures.
 * *************************************************************************************************************
 */

/*----------------------------------------------------------------------------------------------------------------------*/
/* Converts the Vehicle ID number to divided one with lines depending of its length as in the israelian stadards.       */
/* For example: the number with 6 digits will be converted to xxx-xxx.                                                  */
/*                         with 7 digits will be converted to xx-xxx-xx.                                                */
/*                         with 8 digits will be converted to xxx-xx-xxx.                                               */
void VehicleIDToString(char Buf[], int MaxSize, uint32_t VehcleID)
 {
  uint8_t NumDigits;
  NumDigits = IntLog10(VehcleID, 1); /*   "RoundDirrection=0" rounds down "RoundDirrection"=1 rounds up */
  switch (NumDigits)
   {
    case 5: /*  XX-XXX  */
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(__linux__)
      snprintf(Buf, MaxSize, "%02d-%03d", VehcleID / 1000, VehcleID % 1000);
#else
    snprintf(Buf, MaxSize, "%02ld-%03ld", VehcleID / 1000, VehcleID % 1000);
#endif
      break;
  case 6: /*  XXX-XXX  */
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(__linux__)
	      snprintf(Buf, MaxSize, "%03d-%03d", VehcleID / 1000, VehcleID % 1000);
#else
	      snprintf(Buf, MaxSize, "%03ld-%03ld", VehcleID / 1000, VehcleID % 1000);
#endif
      break;
    case 7: /*  XX-XXX-XX  */
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(__linux__)
	      snprintf(Buf, MaxSize, "%02d-%03d-%02d", VehcleID / 100000, (VehcleID % 100000) / 100, VehcleID % 100);
#else
	      snprintf(Buf, MaxSize, "%02ld-%03ld-%02ld", VehcleID / 100000, (VehcleID % 100000) / 100, VehcleID % 100);
#endif
      break;
    case 8: /*  XXX-XX-XXX  */
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(__linux__)
	      snprintf(Buf, MaxSize, "%03d-%02d-%03d", VehcleID / 100000, (VehcleID % 100000) / 1000, VehcleID % 1000);
#else
	      snprintf(Buf, MaxSize, "%03ld-%02ld-%03ld", VehcleID / 100000, (VehcleID % 100000) / 1000, VehcleID % 1000);
#endif
      break;
    case 9: /*  XXX-XXX-XXX  */
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(__linux__)
        snprintf(Buf, MaxSize, "%03d-%03d-%03d", VehcleID / 1000000, (VehcleID % 1000000) / 1000, VehcleID % 1000);
#else
        snprintf(Buf, MaxSize, "%03ld-%03ld-%03ld", VehcleID / 1000000, (VehcleID % 1000000) / 1000, VehcleID % 1000);
#endif
      break;
    default: /* Case undefined. Writing number without any deviation. */
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(__linux__)
	      snprintf(Buf, MaxSize, "%01d", VehcleID);
#else
	      snprintf(Buf, MaxSize, "%01ld", VehcleID);
#endif
      break;
   }
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Converts the Vehicle ID number to divided one with lines depending of its length as in the israelian stadards.       */
/* For example: the number with 6 digits will be converted to xxx-xxx.                                                  */
/*                         with 7 digits will be converted to xx-xxx-xx.                                                */
/*                         with 8 digits will be converted to xxx-xx-xxx.                                               */
/* The number finaly will be formatted and colored as israelian standarted number if the coloring is enabled.           */
void CreateVehIDFormated(char Buf[], int MaxSize, uint32_t VehcleID, bool Colored)
 {
   //  ✡  🔯  🕎 🟌 🇮🇱 🇺🇸
   //  🇮🇮🇱🇱  🇮🇱  🇮 🇱  🇮 🇱  🇮🇮🇮🇱🇮🇮 🇮🇮🇱🇱    🇮🇱🇮🇱  🅘🅛  🇮✡🇱   ✡̲̅    I̅I̲L̅L̲
  int l;
  snprintf(Buf, MaxSize,"%sIL%s", (Colored ? TermBGBlue TermWhite : ""), (Colored ? TermBGYello TermBlack : ""));
  l = strlen(Buf);
  VehicleIDToString(&Buf[l], MaxSize - l, VehcleID);
  l = strlen(Buf);
  snprintf(&Buf[l], MaxSize - l, "%s", (Colored ? TermColorsReset : ""));  
 }


/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Client Name formating Procedures.
 * *************************************************************************************************************
 */

/*----------------------------------------------------------------------------------------------------------------------*/
/* Creates the string with formatted name of a client that can be then shown on a screen through a terminal.            */
void CreateNameFormated(char Buf[], int MaxSize, char const Name[], bool Colored)
 {
  // 웃  👤  🕴🚶🧍🧎⛹⛷🏂🏃🏄🏋🏌🚹
  char const *nc = (Colored ? TermBGWhite TermMagenta : ""); /* Name     Color */
  char const *rc = (Colored ? TermColorsReset         : ""); /* Reset    Color */
  snprintf(Buf, MaxSize - 1, "%s👤%s%s", nc, Name, rc);
 }


/*======================================================================================================================*/
/*
 * *************************************************************************************************************
 **          TimeConvert Functions/Procedures.
 * *************************************************************************************************************
 */

/*----------------------------------------------------------------------------------------------------------------------*/
/*  Converts time to string format.                                                                                     */
void ConvertTime(time_t const *const TimeToConvert, char TimeAsStr[], size_t TimeStrSize, TimeForm_e TimeFormat)
 {
  struct tm tmp;
  localtime_r(TimeToConvert, &tmp);
  switch (TimeFormat)
   {
    case E_CAL_FORMAT: /* Regular Calendar format d/m/y  h:m:s   */
      strftime(TimeAsStr, TimeStrSize, "%d/%b/%Y - %H:%M:%S", &tmp);
     break;
    case E_DBS_FORMAT: /* Database format  yyyy/mm/dd - hh:mm:ss */
      strftime(TimeAsStr, TimeStrSize, "%G/%m/%d %a   %H:%M:%S", &tmp);
     break;
    case E_DUR_FORMAT: /* Duration format  d-h:m:s               */
#if (defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(__linux__)) && (!defined(__arm__))
      snprintf(TimeAsStr, TimeStrSize, "%ldd - %02ld:%02ld:%02ld", *TimeToConvert / (24 * 60 * 60), (*TimeToConvert / (60 * 60)) % 24, (*TimeToConvert / 60) % 60, *TimeToConvert % 60);
#else
      snprintf(TimeAsStr, TimeStrSize, "%lldd - %02lld:%02lld:%02lld", *TimeToConvert / (24 * 60 * 60), (*TimeToConvert / (60 * 60)) % 24, (*TimeToConvert / 60) % 60, *TimeToConvert % 60);
#endif
     break;
   }
 }

/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Price Functions/Procedures.
 * *************************************************************************************************************
 */

/*----------------------------------------------------------------------------------------------------------------------*/
/*  Converts price to string format.                                                                                    */
void ConvertPrice(uint16_t PriceToConvert, char PriceAsString[], size_t PriceStrgSize, PriceFormat_e PriceFormat, bool Formated)
 {
  char const * const units[] = {" ₪ / hour", "₪/h","₪"};
  snprintf(PriceAsString, PriceStrgSize - 1, "%s%d.%02d%s%s%s", (Formated ? PRICE_COLOR : ""), PriceToConvert / 100, PriceToConvert % 100, (Formated ? PRICEUNITS_COLOR : ""), units[PriceFormat % NUM_PRICE_VARIANTS], (Formated ? TermColorsReset : ""));
 }

/*======================================================================================================================*/



#define DEF_INIT_VAL 0xEF45AB12

#define POLYNOM 0x04C11DB7
#define CRC_SHIFT 0
#define MSB_MASK 0xAB25CD87

/*
 * *************************************************************************************************************
 **          CRC Checking Functions / Procedures
 * *************************************************************************************************************
 */
/*----------------------------------------------------------------------------------------------------------------------*/
/*   Calculates CRC from given block of data.                                                                           */
uint32_t FindCRC(uint8_t const *Data, uint8_t Length, uint32_t InitVal)
 {
  uint32_t Result;
  uint32_t vl;
  int i;
  memcpy(&vl, Data, MIN(4, Length));
  Result = InitVal ^ vl;

  for (i = 4; i < Length; i += 4)
   {
    vl = 0;
    memcpy(&vl, &Data[i], MIN(4, (Length - i)));
    if (Result ^ MSB_MASK)
      Result = (Result << CRC_SHIFT) ^ POLYNOM;
    else
      Result = (Result ^ CRC_SHIFT);
   }
  return Result;
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/*   Appends to the end of the data array the calculated CRC from it. The length must include the place of the CRC.     */
/* For example if the data has length 8 the length given as parameter must be 12 = 8 + 4. The CRC has 4 bytes of length.*/
void Add_CRC(uint8_t buf[], size_t len)
 {
  uint32_t CalcCRC;
  CalcCRC = FindCRC(buf, len - CRC_SIZE, DEF_INIT_VAL);
  memcpy(buf + len - CRC_SIZE, &CalcCRC, CRC_SIZE);
 }
/*----------------------------------------------------------------------------------------------------------------------*/
/*  Checks if the CRC is correct.                                                                                       */
/*  For example if the given length is 12 the CRC checking will be made from the first 8 bytes                          */
/*  and the result will be compared to the last 4 bytes.                                                                */
bool CRC_Correct(uint8_t const buf[], size_t len)
 {
  uint32_t CalcCRC, RecvCRC;
  CalcCRC = FindCRC(buf, len - CRC_SIZE, DEF_INIT_VAL);
  memcpy(&RecvCRC, buf + len - CRC_SIZE, CRC_SIZE);
  return CalcCRC == RecvCRC;
 }

/*
 * *************************************************************************************************************
 **          Encoding Decoding Data Functions / Procedures
 * *************************************************************************************************************
 */

/*----------------------------------------------------------------------------------------------------------------------*/
/* This function encodes the packet send to network.                                                                    */
/* Attention !!!                                                                                                        */
/* The last parameter "NetSendData" is given as pointer to pointer to dynamically allocated memory.                     */
/* That means that at the end of the program it must be freed by the procedure "FreeData()"                             */
/* to avoid the memory leakage.                                                                                         */
ssize_t EncodeNetData(uint8_t const *const CustomData, uint8_t Len, uint8_t **NetSendData)
 {

  size_t DataLenFull;
  DataLenFull = Len + CRC_SIZE;
  *NetSendData = (uint8_t *)calloc(DataLenFull, sizeof(uint8_t));
  if (*NetSendData)
   {
    memcpy(*NetSendData, CustomData, Len);
    Add_CRC(*NetSendData, DataLenFull);
    return DataLenFull;
   }

  return 0;
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* This function decodes the packet received from network.                                                              */
bool DecodeNetData(uint8_t const NetRecData[], size_t Len, uint8_t *CustomData)
 {
  if (CRC_Correct(NetRecData, Len))
   {
    memcpy(CustomData, NetRecData, Len - CRC_SIZE);
    return true;
   }
  else
    return false;
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* This Procedure is used for freeing the "**Data" reserved by one of procedure  "EncodeNetData()".                     */
/* No need to check anything before running it because it checks automatically inside if the memory                     */
/* is reserved and sets the pointer to NULL after freeing it.                                                           */
void FreeData(uint8_t **Data)
 {
  if (*Data != NULL)
   {
    free(*Data);
    *Data = NULL;
   }
 }

/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Path Management specific Functions.
 * *************************************************************************************************************
 */

/*----------------------------------------------------------------------------------------------------------------------*/
/* Checks if the path is local and if it is - converts it to full according the path of this executed program.          */
void AdjustPath(char const *OwnProgPathName, char *PathToAdjust, size_t const MaxSize)
 {
  int len = strlen(PathToAdjust);
  char PathDir[PATH_LEN] = {0};
  char *p;
  strncpy(PathDir, OwnProgPathName, sizeof(PathDir) - 1);
  dirname(PathDir);
  if((len == 0) || ((len == 1) && (PathToAdjust[0] = '.'))) /* The path is local and simple */
   {
    strncpy(PathToAdjust, PathDir, MaxSize);
   }
  else
   {
    if((PathToAdjust[0] != '/') && (PathToAdjust[0] != '~')) /* The path is local, but complicated. */
     {
      if(PathDir[strlen(PathDir) - 1] != '/')
       strncat(PathDir, "/", sizeof(PathDir));
      p = (!strncmp(PathToAdjust, "./", 2)) ? (PathToAdjust + 2) : PathToAdjust;
      strncat(PathDir, p, sizeof(PathDir) - 1);
      strncpy(PathToAdjust, PathDir, MaxSize);
     }
   }
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Generates a name of a configuration .ini file according to the name of this running executable file.                 */
/* The extention in this case will be removed. For example:  "program"       ▬▬▬▶   "program.ini".                      */
/*                                                           "program.elf"   ▬▬▬▶   "program.ini"                       */
void GetConfFileName(char const *OwnProgPathName, char *ConfFileName, size_t const MaxSize)
 {
  char *p, *ptnp;
  char TempNamePath[PATH_LEN];
  ptnp = TempNamePath;
  strncpy(TempNamePath, OwnProgPathName, sizeof(TempNamePath) - 1);
  ptnp = basename(ptnp = TempNamePath);
  p = strrchr(ptnp, '.');
  if(p != NULL)
   {
    *p = '\0';
   }
  strncat(ptnp,".ini", sizeof(TempNamePath) - (ptnp - TempNamePath));
  strncpy(ConfFileName, ptnp, MaxSize);
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Gives the path of the folder in which can be stored the configuration files.                                         */
/* Returns "true" if the folder was created. Otherwise returns "false".                                                 */
bool GetConfigPath(char const *OwnProgPathName, char NamePath[], size_t const MaxSize)
 {
  struct stat buffer;
  char *p;
  int l, i;
  struct stat stats;
  bool direxists = false;
  bool result = false;

  char PathDir[PATH_LEN] = {0};
  strncpy(PathDir, OwnProgPathName, (PATH_LEN - 1));
  // p = basename(PathDir);
  p = dirname(PathDir);
  if (!strcmp(p, "."))
   {
    strncpy(NamePath, "./../", MaxSize);
   }
  else
   {
    l = strlen(p);
    l -= (p[l] == '/');

    for (i = l; i >= 0; --i)
     {
      if (p[i] == '/')
        break;
     }

    strncpy(NamePath, p, i + 1);
    NamePath[i + 1] = '\0';
   }

  if (stat(NamePath, &buffer)) /* The previous path doesn't exist. */
   {
    strncpy(NamePath, p, MaxSize);
    strncat(NamePath, "/", MaxSize);
   }

  strncat(NamePath, CONF_DIR_NAME "/", MaxSize);

  /* stat returns 0 on success */
  if (stat(NamePath, &stats) == 0)
   {
    direxists = S_ISDIR(stats.st_mode);
   }

  if (!direxists)
   {
    result = mkdir(NamePath, ACCESSPERMS);
   }

  return result;
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Gives the name with path of the database file.                                                                       */
/* Returns "true" if the folder was created. Otherwise returns "false".                                                 */
bool GetDataBaseFile(char const *OwnProgPathName, char NamePath[], size_t const MaxSize)
 {
  bool result;
  result = GetConfigPath(OwnProgPathName, NamePath, MaxSize);
  strncat(NamePath, DB_FILENAME, MaxSize);
  return result;
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Gives the name with path of the file with the program PID.                                                           */
/* Returns "true" if the folder was created. Otherwise returns "false".                                                 */
bool GetPIDFile(char const *OwnProgPathName, char NamePath[], size_t const MaxSize)
 {
  bool result;
  result = GetConfigPath(OwnProgPathName, NamePath, MaxSize);
  strncat(NamePath, DB_MAN_PID_FILENAME, MaxSize);
  return result;
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Gives the name with path of the file containing the map-shapes of cities, towns, vilages and other geographic places.*/
/* Returns "true" if the folder was created. Otherwise returns "false".                                                 */
bool GetShapeFile(char const *OwnProgPathName, char NamePath[], size_t const MaxSize)
 {
  bool result;
  result = GetConfigPath(OwnProgPathName, NamePath, MaxSize);
  strncat(NamePath, SHP_FILENAME, MaxSize);
  return result;
 }

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(__linux__)  // For Linux
/*----------------------------------------------------------------------------------------------------------------------*/

  char const * const PROC_PATH     = "/proc/";     /* Root Folder contains all running-processes'-information. */
  char const * const STATUS_FILE   = "/status";    /* Text file to see PPID. Must be found the line "PPID:	<PPID>" */
  char const * const COMMON_FILE   = "/comm";      /* Text file containing process name. Contains only string with program name only. "<program_name>" */
  char const * const EXE_FILE      = "/exe";       /* Link file contains path to the program file from which the process was executed. */

/*----------------------------------------------------------------------------------------------------------------------*/  
/* Checks if the copy of the current program is allready running. Returns "true" if running or "false" if not.          */
bool PrevProcCopyRunning(char const *OwnProgPathName, bool ByNameAndPath) /* "false" by name only; "true" by path and name */
 {
  DIR *dir;
  struct dirent *Entry;  
  //char ProcFilePath[100];
  char FilePath[PATH_LEN];
  char RealPath[PATH_LEN];
  char *EndPtr;
  char const *OwnName; 
  char const *FoundName;
  long pid;
  ssize_t len;
  dir = opendir(PROC_PATH);
  bool Result = false;
  
  if(dir == NULL) /* The folder with process' names couldn't be open. It means the function cannot check if any copy of the program is already running. */
   {
    return false; /* If there is no possibility to check if one program copy is running the function returns false to prevent emergency exiting. */
   }
  else /* The folder can be opened. */
   {
    while((Entry = readdir(dir)) != NULL)
     {
      if(Entry->d_type == DT_DIR) /* The found entry is directory */
       {
        errno = 0;
        pid = strtol(Entry->d_name, &EndPtr, 10);
        
        if((EndPtr != Entry->d_name) && (*EndPtr == '\0') && (errno != ERANGE) && ((getpid() != pid))) /* The directory name is a numeric string that means it is a process' PID. The found PID is different from own one that means the process is other. */
         {
          snprintf(FilePath, sizeof(FilePath), "%s%s%s", PROC_PATH, Entry->d_name, EXE_FILE);
          len = readlink(FilePath, RealPath, sizeof(RealPath) - 1);
          if(len != -1)
           {
            RealPath[len] = '\0';

            strncpy(FilePath, OwnProgPathName, sizeof(FilePath) - 1);
            if(ByNameAndPath)
             {
              OwnName = FilePath;
              FoundName = RealPath;
             }
            else
             {
              OwnName = basename(FilePath);
              FoundName = basename(RealPath);
             }
            if(!strcmp(OwnName, FoundName))
             {
              Result = true;
              break;
             }

           }
         }
       }
     }
    closedir(dir);
   }
  return Result;
 }

/*----------------------------------------------------------------------------------------------------------------------*/  
/* Gives the name with the full path of the current running program by checking the /proc/<PID>/exe link file.          */
void GetOwnNamePath(char OwnPathToRet[], size_t const MaxSize)
 {
  char FilePath[PATH_LEN];
  ssize_t len;

  snprintf(FilePath, sizeof(FilePath), "%s%d%s", PROC_PATH, getpid(), EXE_FILE);
  len = readlink(FilePath, OwnPathToRet, MaxSize - 1);
  if(len != -1)
   OwnPathToRet[len] = '\0';
 }


#endif

/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          CRT Console controlling Procedures.
 * *************************************************************************************************************
 */

/*----------------------------------------------------------------------------------------------------------------------*/
/* Set cursor to defined place in the screen.                                                                           */
void MoveCursor(int x, int y)
 {
  printf("\033[%d;%dH", y, x); // Moves cursor to (x,y) place.
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Moves cursor forward.                                                                                                */
void MoveCursFw(int x)
 {
  printf("\033[%dC", x);
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Moves cursor Up.                                                                                                     */
void MoveCursUp(int y)
 {
  printf("\033[%dA", y);
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Moves cursor backward.                                                                                               */
void MoveCursBw(int x)
 {
  printf("\033[%dD", x);
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Moves cursor down.                                                                                                   */
void MoveCursDn(int y)
 {
  printf("\033[%dB", y);
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Sets cursor place in the line were it exists.                                                                        */
void MoveCursToCol(int col)
 {
  printf("\033[%dG", col);
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Clears line in console.                                                                                              */
void ClearLine(LnPrt_e lp)
 {
  printf("\033[%dK", lp);
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Prints horizontal scale bar.                                                                                         */
void PrintHorizScale(uint32_t ScaleLength, uint32_t FilledLen, char *Colors[], char *Symbols[], uint32_t MaxValue, uint32_t Value, char Units[])
 {
  uint32_t i;
  uint32_t PermLen;
  char buf[20], b[10];
  bool d;
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(__linux__)
  snprintf(buf, sizeof(buf), "%d", MaxValue);
#else
  snprintf(buf, sizeof(buf), "%ld", MaxValue);
#endif
  snprintf(b, sizeof(b), " %%%dd%%s ", (uint8_t)strlen(buf));
  snprintf(buf, sizeof(buf), b, Value, Units);
  printf("%s%s", Colors[1], buf);
  PermLen = ScaleLength - strlen(buf);
  if (PermLen)
    for (i = 0; i < ScaleLength; i++)
    {
      d = (i < FilledLen);
      printf("%s%s%s", Colors[d], Symbols[d], TermColorsReset);
    }
  fflush(stdout);
 }

/*======================================================================================================================*/

#if defined(A) // defined(__x86_64__) || defined(_M_X64)
voit ttt()
 {
 }
#endif

//  ₙⁿ𐄁 ⁰¹²³⁴⁵⁶⁷⁸  ₀₁₂₃₄₅₆₇₈₉  ⩽⩾
//  ₒᵤₜ ᵢₙ
//  ≡≣≡≣
// ㎐㎑㎒㎓㎔
//          ------
//  ◀ ▶   ◀⸻ ⸺▶   ◀▬▬▬ ▬▬▬▶  <-- -->  🡄🬋🬋🡆  ⇶   ⇇⇉
//  ▤▥▦▧▨▩▪▫░▒▓
//  🔏🔐🔑🔒🔓 🚏  🚥🚦🚪

// ✡  🔯 🕎 🟌
//  ✡  🔯  🕎 🟌 🇮🇱 🇺🇸
//  🇮🇮🇱🇱  🇮🇱  🇮 🇱  🇮 🇱  🇮🇮🇮🇱🇮🇮 🇮🇮🇱🇱    🇮🇱🇮🇱  🅘🅛  🇮✡🇱   ✡̲̅    I̅I̲L̅L̲

//  🏙
//  🌳
//  🌴
//  🏜


//  🌍🌎🌏🌐🚗🚶🅿️📁💸🖨⌖🧭📍°

// 웃  👤  🕴🚶🧍🧎⛹⛷🏂🏃🏄🏋🏌🚹