#include "Parking.hpp"
#include "main.hpp"
#include "Errors.hpp"
#include "Processes.hpp"

#include <semaphore.h>
#include <unistd.h>
#include <iostream>
#include <sys/sem.h>
#include <sys/shm.h>

#include <vector>
#include <cmath>

#include <ogrsf_frmts.h>  /* Requires GDAL Library installation. See site "https://gdal.org/en/stable/doxygen/ogrsf__frmts_8h.html" */  
#include <ogr_spatialref.h>


/**
 * 
 *        To install GDAL library: 
 *    
 *        sudo apt update
 *        sudo apt install gdal-bin libgdal-dev
 *        (conda install -c conda-forge gdal)

 */



#define PARK_PROC_NAME     (char *)"Parking"     /* Parking process name*/


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

class Parking_c: public Process_c
 {
  public:
    Parking_c(char ProcName[], key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType);
    virtual ~Parking_c();
    virtual void OnRunProcess();
 };


void ParkingProc(key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType)
 {
  Parking_c Park_Process(PARK_PROC_NAME, sh_mem_key, sem_name, ProcType);
  Park_Process.RunProcess();
 }


void Parking_c::OnRunProcess()
 {
  Process_c::OnRunProcess();
 };



Parking_c::Parking_c(char ProcName[], key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType)
 :Process_c(ProcName, sh_mem_key, sem_name, ProcType)
 {

 }

Parking_c::~Parking_c()
 {

 }




 ///////////////////////////////////////////////////////////////////////////////////


bool GetFeatureLatinName(OGRFeature *poFeature, std::string &CityName);

// The "ShapeFileName" should be got by the function "TaskControl_ShSM_c::GetSHPFileName()".
bool DetectCity(GPS_Cords_s Cords, std::string &CityName,  std::string &ShapeFileName)
 {
  // bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  // bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */

  OGRPoint Point(0,0);
  OGRwkbGeometryType FigureType;
  OGRGeometry *poGeometry;
  int NumLayers;
  int NumFeatures;
  int FeatureNo;
  bool Result = false;

  double x, y;
  std::string FileName;
  std::cout << "Running City Detection Function ...\n\r";
  /* Define the Source Spatial Reference (New Israel Grid / EPSG 2039) */
  OGRSpatialReference oSourceSRS;
  if (oSourceSRS.importFromEPSG(2039) != OGRERR_NONE) 
   {
    std::cerr << "Error: Could not import EPSG:2039" << std::endl;
    return false;
   }
   /* Define the Target Spatial Reference (WGS 84 / EPSG 4326) */
  OGRSpatialReference oTargetSRS;
  if (oTargetSRS.importFromEPSG(4326) != OGRERR_NONE) 
   {
    std::cerr << "Error: Could not import EPSG:4326" << std::endl;
    return false;
   }
  /* CRITICAL FOR GDAL 3+: Enforce traditional Longitude/Latitude order axis mapping */
  oTargetSRS.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);

  /* Create the Transformation Object */
  OGRCoordinateTransformation* poCT = OGRCreateCoordinateTransformation(&oSourceSRS, &oTargetSRS);
  if (poCT == nullptr) 
   {
    std::cerr << "Error: Transformation object creation failed." << std::endl;
    return false;
   }
  
  OGRCoordinateTransformation* backPoCT = OGRCreateCoordinateTransformation(&oTargetSRS, &oSourceSRS);
  if (backPoCT == nullptr) 
   {
    std::cerr << "Error: Transformation object creation failed." << std::endl;
    return false;
   }

  GDALAllRegister();
  GDALDataset *poDS = (GDALDataset*) GDALOpenEx(ShapeFileName.c_str(), GDAL_OF_VECTOR, nullptr, nullptr, nullptr);

  if (poDS == nullptr) 
   { 
    std::cerr << "The shapefile couldn't be detected. Recheck the \"" << ShapeFileName << "\" file." << "\n\r";
    return false; 
   }

  NumLayers = poDS->GetLayerCount();
  OGRLayer *poLayer = poDS->GetLayer(0);
  poLayer->ResetReading();
  OGRFeature *poFeature;
  std::cout << "The shape contains " << NumLayers << " layers.\n\r";

  x = Cords.Longitude;
  y = Cords.Latitude;

  std::cout << "Converting Given Coordinates ...\n\r";
  PrintGPSCords(GPS_Cords_s(x,y));
  std::cout << "  ->  ";
  //backPoCT->Transform(1, &x, &y);
  std::cout << "x=" << x << ", y=" << y;
  std::cout << "\n\r";

  Point = OGRPoint(x,y);
  NumFeatures = poLayer->GetFeatureCount();
  std::cout << "The layer contains " << NumFeatures << " features.\n\r";
  for(FeatureNo = 0;((poFeature = poLayer->GetNextFeature()) != nullptr) && (FeatureNo < NumFeatures); FeatureNo++) 
   {
    // GetFeatureLatinName(poFeature, CityName);
    // std::cout << "Feature " << FeatureNo << "    " << CityName << "\n\r";
    poGeometry = poFeature->GetGeometryRef();
    if(poGeometry != nullptr)
     {
      FigureType = poGeometry->getGeometryType();
      // std::string FigureName = poGeometry->getGeometryName();
      // std::cout << "Geometry type is: " << FigureName << "  " << FigureType << "\n\r";
      OGRwkbGeometryType Wkb = wkbFlatten(FigureType);
      if(Wkb == wkbPolygon) 
       {
        OGRPolygon *PoPolygon = poGeometry->toPolygon();
        Result |= PoPolygon->IsPointOnSurface(&Point);
        if(Result) /* The Polygon was found. */
         {
          std::cout << "Polygon Found" << "\r\n";
          break;
         }
       }
      else if(Wkb == wkbMultiPolygon)
       {
        OGRMultiPolygon* poMultiPoly = poGeometry->toMultiPolygon();
        int numPolygons = poMultiPoly->getNumGeometries();
        OGRGeometry* subPoGeometry;
        OGRPolygon *SubPoPolygon;
        for(int i=0; i < numPolygons; i++)
         {
          subPoGeometry = poMultiPoly->getGeometryRef(i);
          SubPoPolygon = subPoGeometry->toPolygon();
          Result |= SubPoPolygon->IsPointOnSurface(&Point);
          if(Result) /* The SubPolygon was found. */
           {
            std::cout << "SubPolygon Found" << "\r\n";
            break;
           }
         }
        if(Result) /* The MultiPolygon was found. */
         {
          std::cout << "MultiPolygon Found" << "\r\n";
          break;
         }
       }
     }
    OGRFeature::DestroyFeature(poFeature);
   }
  std::cout << "Last Feature No: " << FeatureNo << " The number of Features " << NumFeatures << "\n\r";
  if((Result) && (FeatureNo < NumFeatures))
   {
    Result = GetFeatureLatinName(poFeature, CityName);
    // int Index;
    // const OGRFieldDefn *FieldRef;
    // OGRFieldType FieldType;
    // Index = poFeature->GetFieldIndex(SHP_FIELD_NAME);
    // if(Index > 0)  /* The field with required name was found */
    //  {

    //   FieldRef = poFeature->GetFieldDefnRef(Index);
    //   if(FieldRef != nullptr) /* The fild realy exists. */
    //    {
    //     FieldType = FieldRef->GetType();
    //     if(FieldType == OFTString) /* The field is realy string. */
    //      CityName = poFeature->GetFieldAsString(Index);      
    //     else  /* The field isn't a string at all.*/
    //      Result = false;
    //    }
    //   else  /* The fild doesn't exist at all. */
    //    Result = false;
    //  }
    // else  /* The filed with required name wasn't found. */
    //  {
    //   Result = false;
    //  }
   }
  
  GDALClose(poDS);
  OGRCoordinateTransformation::DestroyCT(backPoCT);
  OGRCoordinateTransformation::DestroyCT(poCT);

  return Result;
 }



bool GetFeatureLatinName(OGRFeature *poFeature, std::string &CityName)
 {
  int Index;
  bool Result = true;
  const OGRFieldDefn *FieldRef;
  OGRFieldType FieldType;
  

  Index = poFeature->GetFieldIndex(SHP_FIELD_NAME);

  if(Index > 0)  /* The field with required name was found */
   {
    FieldRef = poFeature->GetFieldDefnRef(Index);
    if(FieldRef != nullptr) /* The fild realy exists. */
     {
      FieldType = FieldRef->GetType();
      if(FieldType == OFTString) /* The field is realy string. */
       CityName = poFeature->GetFieldAsString(Index);      
      else  /* The field isn't a string at all.*/
       Result = false;
     }
    else  /* The fild doesn't exist at all. */
     Result = false;
   }
  else  /* The filed with required name wasn't found. */
   {
    Result = false;
   }
  if(Result == false)
   {
    CityName = "";
   }
  return Result;
 }