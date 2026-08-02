
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

/*======================================================================================================================*/

char const *const ResultColors[] = {TermRed, TermGreen, TermYello, TermCyan, TermMagenta};

/*======================================================================================================================*/

#define EARTH_DIAMETER 12742.018 /* km */          /* Mean earth diameter */
#define EARTH_RADIUS (EARTH_DIAMETER / 2) /* km */ /* Mean earth radius   */

#define EARTH_DIAMETER_P 12713.505 /* km */ /* Polar earth diameter */
#define EARTH_DIAMETER_E 12756.274 /* km */ /* Equatorial earth diameter */

#define EARTH_RADIUS_P (EARTH_DIAMETER_P / 2) /* km */ /* Polar earth radius */
#define EARTH_RADIUS_E (EARTH_DIAMETER_E / 2) /* km */ /* Equatorial earth radius */

char const *LATSGN[] = {"N", "S"};  // Latitude  sign "N" (North) in case of positive, "S" (South) in case of negative.
char const *LONGSGN[] = {"E", "W"}; // Longitude sign "E" (East)  in case of positive, "W" (West)  in case of negative.

/*======================================================================================================================*/
/*
 * *************************************************************************************************************
 **          Additional mathematic Functions.
 * *************************************************************************************************************
 */

/*----------------------------------------------------------------------------------------------------------------------*/
/* Rases numbers to the square power.                                                                                   */
double sqr(double x)
 {
  return x * x;
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Generates random numbers in 64 bit size.                                                                            */
uint64_t RandGenLongLong()
 {
  return ((uint64_t)rand() << 32) | ((uint64_t)rand());
 }

int32_t IntLog10(uint32_t Value, uint8_t RoundDirrection) /*   "RoundDirrection=0" rounds down "RoundDirrection"=1 rounds up */
 {
  int32_t Result;
  /*  If rounding up is required the comparison result will be "(Value!=0)". If tequired the rounding down the comparison result will be "(Value>=10)".   */
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

void AngToDebMinSecDec(double Angle, AngDegMinSec_s *ConvAng)
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

void CordsToString(char Buf[], int MaxSize, GPS_Cords_s GPSCords)
 {
  AngDegMinSec_s LatConvAng, LongConvAng;
  AngToDebMinSecDec(GPSCords.Latitude, &LatConvAng);
  AngToDebMinSecDec(GPSCords.Longitude, &LongConvAng);
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(__linux__)  // For Linux
  snprintf(Buf, MaxSize, "%d˚%d'%d\".%08d LONG %d˚%d'%d\".%08d LAT", LongConvAng.Deg, LongConvAng.Min, LongConvAng.Sec, (int32_t)(LongConvAng.Dec * 100000000), LatConvAng.Deg, LatConvAng.Min, LatConvAng.Sec, (int32_t)(LatConvAng.Dec * 100000000));
#else  // For ARM
  snprintf(Buf, MaxSize, "%d˚%d'%d\".%08ld LONG %d˚%d'%d\".%08ld LAT", LongConvAng.Deg, LongConvAng.Min, LongConvAng.Sec, (int32_t)(LongConvAng.Dec * 100000000), LatConvAng.Deg, LatConvAng.Min, LatConvAng.Sec, (int32_t)(LatConvAng.Dec * 100000000));
#endif

  // snprintf(Buf, MaxSize, "%3.8lf˚ lat %3.8lf˚ long", GPSCords.Latitude, GPSCords.Longitude);
 }

void PrintGPSCords(GPS_Cords_s CordsToPrint)
 {
  char buf[200];
  CordsToString(buf, sizeof(buf), CordsToPrint);
  printf("%s", buf);
 }

/*======================================================================================================================*/

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
    case E_DBS_FORMAT: /* Database fromat  yyyy/mm/dd - hh:mm:ss */
      strftime(TimeAsStr, TimeStrSize, "%G/%m/%d %a   %H:%M:%S", &tmp);
     break;
    case E_DUR_FORMAT: /* Duration format  d-h:m:s               */
#if (defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(__linux__)) && (!defined(__arm__))
      snprintf(TimeAsStr, TimeStrSize, "%ldd %02ld:%02ld:%02ld", *TimeToConvert / (24 * 60 * 60), (*TimeToConvert / (60 * 60)) % 24, (*TimeToConvert / 60) % 60, *TimeToConvert % 60);
#else
      snprintf(TimeAsStr, TimeStrSize, "%lldd %02lld:%02lld:%02lld", *TimeToConvert / (24 * 60 * 60), (*TimeToConvert / (60 * 60)) % 24, (*TimeToConvert / 60) % 60, *TimeToConvert % 60);
#endif
     break;
   }
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
uint32_t FindCRC(uint8_t *Data, uint8_t Length, uint32_t InitVal)
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
bool CRC_Correct(uint8_t buf[], size_t len)
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
bool DecodeNetData(uint8_t NetRecData[], size_t Len, uint8_t *CustomData)
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
 **          DataBase specific Functions.
 * *************************************************************************************************************
 */

void AdjustPath(char const *OwnProgName, char *PathToAdjust)
 {
  int len = strlen(PathToAdjust);
  char PathDir[PATH_LEN] = {0};
  char *p;
  strcpy(PathDir, OwnProgName);
  dirname(PathDir);
  if((len == 0) || ((len == 1) && (PathToAdjust[0] = '.'))) /* The path is local and simple */
   {
    strcpy(PathToAdjust, PathDir);
   }
  else
   {
    if((PathToAdjust[0] != '/') && (PathToAdjust[0] != '~')) /* The path is local, but complicated. */
     {
      if(PathDir[strlen(PathDir) - 1] != '/')
       strcat(PathDir, "/");
      p = (!strncmp(PathToAdjust, "./", 2)) ? (PathToAdjust + 2) : PathToAdjust;
      strcat(PathDir, p);
      strcpy(PathToAdjust, PathDir);
     }
   }
 }

void GetConfFileName(char const *OwnProgName, char *ConfFileName)
 {
  char *p;
  strcpy(ConfFileName, OwnProgName);
  ConfFileName = basename(ConfFileName);
  p = strrchr(ConfFileName, '.');
  if(p != NULL)
   {
    *p = '\0';
   }
  strcat(ConfFileName,".ini");
 }

bool GetConfigPath(char const *OwnProgName, char NamePath[])
 {
  struct stat buffer;
  char *p;
  int l, i;
  struct stat stats;
  bool direxists = false;
  bool result = false;

  char PathDir[PATH_LEN] = {0};
  strncpy(PathDir, OwnProgName, PATH_LEN);
  // p = basename(PathDir);
  p = dirname(PathDir);
  if (!strcmp(p, "."))
   {
    strcpy(NamePath, "./../");
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
    strcpy(NamePath, p);
    strcat(NamePath, "/");
   }

  strcat(NamePath, CONF_DIR_NAME "/");

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

bool GetDataBaseFile(char const *OwnProgName, char NamePath[])
 {
  bool result;
  result = GetConfigPath(OwnProgName, NamePath);
  strcat(NamePath, DB_FILENAME);
  return result;
 }

bool GetPIDFile(char const *OwnProgName, char NamePath[])
 {
  bool result;
  result = GetConfigPath(OwnProgName, NamePath);
  strcat(NamePath, DB_MAN_PID_FILENAME);
  return result;
 }

bool GetShapeFile(char const *OwnProgName, char NamePath[])
 {
  bool result;
  result = GetConfigPath(OwnProgName, NamePath);
  strcat(NamePath, SHP_FILENAME);
  return result;
 }
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

