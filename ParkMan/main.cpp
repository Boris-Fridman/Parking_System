#include <iostream>
#include "CommonData.h"
#include <cmath>
#include "DataBase.hpp"
#include "Network.hpp"
#include "Parking.hpp"


/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Main Function from which the program starts running. 
 * *************************************************************************************************************
 */
/*----------------------------------------------------------------------------------------------------------------------*/
/*  Main function from which the program starts running.                                                                */

int main()
{
 GPS_Cords_s TelAviv = {32.0853, 34.7818}, Jerusalem = {31.7683, 35.2137};
 double d;
 d = GetDistance(TelAviv, Jerusalem);
 printf("%f\n\r",d);
 return 0;
}