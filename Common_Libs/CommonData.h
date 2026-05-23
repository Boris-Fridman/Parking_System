
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
#define MAX(X, Y) ( (X) > (Y) ? (X) : (Y) )           /* The macro returning the biggest from the two values.  */
#define MIN(X, Y) ( (X) < (Y) ? (X) : (Y) )           /* The macro returning the smallest from the two values. */

/**
 * Div macros
 */
#define DIV_RND(X,Y)    ( ((X) + (Y) / 2) / (Y) )     /* Deviation with rounding.      10/6 will give 2 and 10/3 will give 3  */
#define DIV_RND_UP(X,Y) ( ((X) + (Y) - 1) / (Y) )     /* Deviation with rounding up.   10/6 will give 2 and 10/3 will give 4  */
#define DIV_RND_DN(X,Y) ( (X) / (Y)             )     /* Deviation with rounding down. 10/6 will give 1 and 10/3 will give 3 Regular deviation equal to regular "/". Is defined for compilation the previous macros deviations */


#define EARTH_DIAMETER 12742                     /* km */ /* Mean earth diameter */
#define EARTH_RADIUS   (EARTH_DIAMETER / 2)      /* km */ /* Mean earth radius   */

#define PARK_NAME_LEN 20


/*======================================================================================================================*/

/**
 * Macro for preventing warnings in case of unused variables.
 */
#if !defined(UNUSED)
#define UNUSED(X) (void)X      /* To avoid gcc/g++ warnings */
#endif /* UNUSED */

/*======================================================================================================================*/


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
 struct Parking_s
  {
   GPS_Cords_s Cords;
   uint16_t MaxNumPlaces;
   uint16_t NumPlaces;
   uint16_t Price;            /*  0.01₪ / min  */
   char Name[PARK_NAME_LEN];
  }
#ifndef __cplusplus  
Parking_s
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

/*======================================================================================================================*/


double GetDistance(GPS_Cords_s p1, GPS_Cords_s p2);



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
