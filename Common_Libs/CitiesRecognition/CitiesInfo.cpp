#include "CitiesInfo.hpp"
#include "cmath"

double Atan2(double y, double x)
 {
  double t;
  t = atan2(y, x) + (2*M_PI);
  t = fmod(t, 2 * M_PI);
  return t;
 }

PointState_e PointInPoly(GPS_Poly_t BoundingPoly, GPS_Cords_s Cords)
 {
  std::size_t i;
  double x1, x2, y1, y2, yt, xt;
//   double a1, a2;
  double angle, dang;
  bool inside, outside;
  std::size_t sz;
  sz = BoundingPoly.size();
  for(i = 0, angle = 0; i < BoundingPoly.size(); i++)
   {
    x1 = BoundingPoly[ i          ].Longitude - Cords.Longitude;
    y1 = BoundingPoly[ i          ].Latitude  - Cords.Latitude ;
    x2 = BoundingPoly[(i + 1) % sz].Longitude - Cords.Longitude;
    y2 = BoundingPoly[(i + 1) % sz].Latitude  - Cords.Latitude ;

    // a1 = Atan2(y1, x1);
    // a2 = Atan2(y2, x2);
    // dang = a2 - a1;

    xt = x1 * x2 + y1 * y2;
    yt = x1 * y2 - x2 * y1;

    if((xt == 0) && (yt == 0))  /* Checking if the tested point is on any bounding point limit. if this happens arctg(yt/xt) means arctg(0/0) that isn't defined. In addition it means that the point is placed on the bounding corner. */
     return ONBOUND_E;  /* The tested point exists on the corner of the bounding polynom. Nothing to check else. */

    dang = atan2(yt, xt);

    if(std::abs(dang) == M_PI) /* Checking if the angle is equivalent to 180° or π radians that means that the point exist exactely on the bounding line. */
     return ONBOUND_E;  /* The point exists on the line of the bounding polynom. Nothing to check else. */
    
    angle += dang;
   }

  angle = std::abs(angle);  // In case of the oposite rotation the angle will be -2π instead of 2π.
  outside = std::abs(angle             ) < PERMITED_ERROR;  //  Outsize
  inside  = std::abs(angle - (2 * M_PI)) < PERMITED_ERROR;  //  Insize
  if(outside) return OUTSIZE_E;
  else if(inside) return INSIDE_E;
  else return ONBOUND_E;
 }


void DetectCityFromGPSCords(CityData_s const CitiesList[], ssize_t const NumSities, GPS_Cords_s Cords, std::string &CityName, ssize_t &Number, IntArr_t &BoundingCities)
 {
  ssize_t i; 
  //ssize_t boundind = -1;
  PointState_e result;
  for(i = 0, Number = -1, BoundingCities.clear(); i < NumSities; i++)
   {
    result = PointInPoly(CitiesList[i].CityCords, Cords);
    if(result == INSIDE_E)
     {
      CityName = CitiesList[i].Name;
      Number = i;
      break;
     }
    else 
     if( /*(boundind < 0) &&*/ (result == ONBOUND_E))
      {
       //boundind = i;
       BoundingCities.push_back(i);
      }
   }
  // if(i == NumSities)  // No city city was detected.
  //  {
  //   Number = boundind;
  //   if(boundind >= 0) // Was detected city in which the point was put on the bond.
  //    {
  //     CityName = CitiesList[boundind].Name;
  //    }
  //  }

 }