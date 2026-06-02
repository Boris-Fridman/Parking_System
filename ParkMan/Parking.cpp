#include "Parking.hpp"
#include <unistd.h>




Customer_c::Customer_c()
 {

 }


City_c::City_c()
  :CenterPlace({0,0})
 {

 }

City_c::~City_c()
 {

 }



 void ParkingProc()
  {
   sleep(40);  // Was added for test only. In the future will be removed.
  }

