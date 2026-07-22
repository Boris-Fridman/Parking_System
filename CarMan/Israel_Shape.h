#ifndef ____Israel_Shape_h__
#define ____Israel_Shape_h__

/****************************************************************************************************
 * Was defined for compilation under PC only. In case of BeagleBone it is not used in compilation.  *
 ****************************************************************************************************
 */


#include "CommonData.h"

typedef enum PointState_e
 {
  OUTSIZE_E,
  ONBOUND_E,
  INSIDE_E
 }PointState_e;

#define PERMITED_ERROR 0.01

extern const GPS_Cords_s IsraelShape[];

PointState_e PointInPoly(const GPS_Cords_s BoundingPoly[], size_t PolySize, GPS_Cords_s Cords);

extern const size_t NUM_IL_SHAPE_PNT;


#endif  //  ____Israel_Shape_h__

