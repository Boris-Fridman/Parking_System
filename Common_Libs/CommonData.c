
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

#include <libgen.h>

/*======================================================================================================================*/

char const * const ResultColors[] = {TermRed, TermGreen, TermYello, TermCyan, TermMagenta};

/*======================================================================================================================*/

#define EARTH_DIAMETER 12742.018                      /* km */ /* Mean earth diameter */
#define EARTH_RADIUS   (EARTH_DIAMETER / 2)           /* km */ /* Mean earth radius   */

#define EARTH_DIAMETER_P 12713.505                    /* km */ /* Polar earth diameter */
#define EARTH_DIAMETER_E 12756.274                    /* km */ /* Equatorial earth diameter */

#define EARTH_RADIUS_P (EARTH_DIAMETER_P / 2)        /* km */ /* Polar earth radius */
#define EARTH_RADIUS_E (EARTH_DIAMETER_E / 2)        /* km */ /* Equatorial earth radius */

char const *LATSGN[] = {"N", "S"};   // Latitude  sign "N" (North) in case of positive, "S" (South) in case of negative.
char const *LONGSGN[] = {"E", "W"};  // Longitude sign "E" (East)  in case of positive, "W" (West)  in case of negative.

/*======================================================================================================================*/
/*
 * *************************************************************************************************************
 **          Additional mathematic Functions.
 * *************************************************************************************************************
 */

double sqr(double x)
 {
  return x * x;
 }

/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Coordinates and distance Functions.
 * *************************************************************************************************************
 */

void GPSToSpace(Space_Cords_s *SpaceCords, GPS_Cords_s GPS_Data)
 {
  SpaceCords->x = EARTH_RADIUS_E * cos(GPS_Data.Latitude * M_PI /  180) * cos(GPS_Data.Longitude * M_PI / 180);
  SpaceCords->y = EARTH_RADIUS_E * cos(GPS_Data.Latitude * M_PI /  180) * sin(GPS_Data.Longitude * M_PI / 180);
  SpaceCords->z = EARTH_RADIUS_P * sin(GPS_Data.Latitude * M_PI /  180);
 }

double GetDistance(GPS_Cords_s p1, GPS_Cords_s p2)
 {
  Space_Cords_s s1, s2;
  double LinDist, ArcDist;
  GPSToSpace(&s1, p1);
  GPSToSpace(&s2, p2);
  LinDist = sqrt(sqr(s1.x - s2.x) + sqr(s1.y - s2.y) + sqr(s1.z - s2.z));
  ArcDist = asin( LinDist / EARTH_RADIUS / 2) * EARTH_RADIUS * 2;
  return ArcDist;
 }


void CordsToString(char Buf, int MaxSize, GPS_Cords_s GPSCords)
 {
  snprintf(Buf, MaxSize, "%3.8lf˚ lat %3.8lf˚ long", GPSCords.Latitude, GPSCords.Longitude);
 }

void PrintGPSCords(GPS_Cords_s CordsToPrint)
 {
  char buf[50];
  CordsToString(buf, sizeof(buf), CordsToPrint);
  printf(buf);
 }

/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          DataBase specific Functions.
 * *************************************************************************************************************
 */

bool GetConfigPath(int const argc, char const *argv[], char NamePath[])
 {
  struct stat buffer;
  char *p;
  int l, i;
  struct stat stats;
  bool direxists = false;
  bool result = false;

  UNUSED(argc);

  char PathDir[PATH_LEN] = {0};
  strncpy(PathDir, argv[0], PATH_LEN);
  //p = basename(PathDir);
  p = dirname(PathDir);
  if(!strcmp(p,"."))
   {
    strcpy(NamePath, "./../");
   }
  else
   {
    l = strlen(p);
    l -= (p[l] == '/');
  
    for(i = l; i >= 0; --i)
     {
      if(p[i] == '/')
       break;
     }
    
    strncpy(NamePath, p, i+1);
    NamePath[i+1] = '\0';
   }


  if(stat(NamePath, &buffer)) // The previous path doesn't exist.
   {
    strcpy(NamePath, p);
    strcat(NamePath, "/");
   }

  strcat(NamePath, CONF_DIR_NAME"/");
  
  // stat returns 0 on success
  if(stat(NamePath, &stats) == 0) 
   {
    direxists = S_ISDIR(stats.st_mode);
   }

  if(!direxists)
   {
    result = mkdir(NamePath, ACCESSPERMS);
   }
  
  return result;
 }


bool GetDataBaseFile(int const argc, char const *argv[], char NamePath[])
 {
  bool result;
  result = GetConfigPath(argc, argv, NamePath);
  strcat(NamePath, DB_FILENAME);
  return result;
 }

bool GetPIDFile(int const argc, char const *argv[], char NamePath[])
 {
  bool result;
  result = GetConfigPath(argc, argv, NamePath);
  strcat(NamePath, DB_MAN_PID_FILENAME);
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
  printf("%s%s",Colors[1], buf);
  PermLen = ScaleLength - strlen(buf);
  if(PermLen)
   for(i = 0; i < ScaleLength; i++)
    {
     d = (i < FilledLen);
     printf("%s%s%s", Colors[d], Symbols[d], TermColorsReset);
    }
  fflush(stdout);

 }


/*======================================================================================================================*/

#if defined(A) //defined(__x86_64__) || defined(_M_X64)
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
