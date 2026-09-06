
/*======================================================================================================================*/

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
#include <algorithm>
#include "Vocobulary.hpp"

#include "Configuration.hpp"

#include <thread>

/*======================================================================================================================*/

/**
 * 
 *        To install GDAL library: 
 *        
 *        sudo apt update
 *        sudo apt install gdal-bin libgdal-dev
 *        (conda install -c conda-forge gdal)

 */

/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Functions / Procedures for internal usage.
 * *************************************************************************************************************
 */

/* The "ShapeFileName" should be got by the function "TaskControl_ShSM_c::GetSHPFileName()". */
bool DetectCity(GPS_Cords_s Cords, std::string &CityName, uint32_t &RegionCode, uint32_t &EdRegCode,  std::string &ShapeFileName);

bool StringsAreEqual(std::string str1, std::string str2);
void RemoveUnneededChars(std::string &StringToClear);
void ReplaceSubStrings(std::string &StringToCorrect);

bool GetFeatureLatinName(OGRFeature *poFeature, std::string &CityName);
bool GetFeatureRegionCodes(OGRFeature *poFeature, uint32_t &RegionCode, uint32_t &EdRegCode);


/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Region detections Functions / Procedures 
 * *************************************************************************************************************
 */

/*----------------------------------------------------------------------------------------------------------------------*/
/* Detects the City according the given Map Shape File and the given GPS Coordinates. Returns name and regions codes    */
bool DetectCity(GPS_Cords_s Cords, std::string &CityName, uint32_t &RegionCode, uint32_t &EdRegCode, std::string &ShapeFileName)
 {
  // bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */

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

  char CordsBuf[200];
  CreateCordsFormatted(CordsBuf, sizeof(CordsBuf), GPS_Cords_s(x,y), StdOutNoPiping);
  std::cout << "Converting Given Coordinates ...\n\r";
  //backPoCT->Transform(1, &x, &y);
  std::cout << CordsBuf << "  ->  " << "x=" << x << ", y=" << y << "\n\r";

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
        for(int i = 0; i < numPolygons; i++)
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
    if(Result)
     Result = GetFeatureRegionCodes(poFeature, RegionCode, EdRegCode);
   }
  
  GDALClose(poDS);
  OGRCoordinateTransformation::DestroyCT(backPoCT);
  OGRCoordinateTransformation::DestroyCT(poCT);

  return Result;
 }


/*----------------------------------------------------------------------------------------------------------------------*/
/* Returns the Name in latin letters of the Region given in the feature containing region's bounding polygon.           */
bool GetFeatureLatinName(OGRFeature *poFeature, std::string &CityName)
 {
  int Index;
  bool Result = true;
  const OGRFieldDefn *FieldRef;
  OGRFieldType FieldType;
  const char *PlaceName;

  PlaceName = GetColumnNameWithGeolocationName();
  //PlaceName = SHP_FIELD_NAME;  // For test only.
  //std::cout << "Detecting City name in the column named: " << "\"" << PlaceName << "\"" <<"\n\r";
  Index = poFeature->GetFieldIndex(PlaceName);

  if(Index >= 0)  /* The field with required name was found */
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

/*----------------------------------------------------------------------------------------------------------------------*/
/* Returns the Main and the Edditional Region Codes from the given geometry feature conatins the retion's polygon.      */
bool GetFeatureRegionCodes(OGRFeature *poFeature, uint32_t &RegionCode, uint32_t &EdRegCode) // At this moment the function returns only the region code. The Editional Region code will be implemented later.
 {
  int Index;
  bool Result = true;
  const OGRFieldDefn *FieldRef;
  OGRFieldType FieldType;
  const char *OSMIDName;

  OSMIDName = GetColumnNameWithOSMID();
  //OSMIDName = SHP_OSM_ID;  // For test only.
  //std::cout << "Detecting OSM ID in the column named: " << "\"" << OSMIDName << "\"" <<"\n\r";
  Index = poFeature->GetFieldIndex(OSMIDName);
  
  UNUSED(EdRegCode);
  if(Index >= 0)  /* The field with required name was found */
   {
    FieldRef = poFeature->GetFieldDefnRef(Index);
    
    if(FieldRef != nullptr) /* The fild realy exists. */
     {
      FieldType = FieldRef->GetType();
      if(FieldType == OFTString) /* The field is realy string. */
       RegionCode = std::stoul(poFeature->GetFieldAsString(Index));      
      else  
       if(FieldType == OFTInteger) /* The field isn't a string, but maybe an integer. */
        RegionCode = poFeature->GetFieldAsInteger(Index);      
       else /* The field is neither string nor an integer. */
        Result = false;
     }
    else         /* The fild doesn't exist at all. */
     Result = false;
   }
  else  /* The filed with required name wasn't found. */
   {
    Result = false;
   }
  if(Result == false)
   {
    RegionCode = 0;
   }
  return Result;
 }

/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Regions' names spelling checking Functions / Procedures 
 * *************************************************************************************************************
 */

/*----------------------------------------------------------------------------------------------------------------------*/
/* Compares the names of the cities given as strings if they are equal by taking in account the possible alternatives.  */
bool StringsAreEqual(std::string str1, std::string str2)
 {
  RemoveUnneededChars(str1);
  RemoveUnneededChars(str2);
  ReplaceSubStrings(str1);
  ReplaceSubStrings(str2);
  // std::cout << TermYello << str1 << " = ? = " << str2 << TermColorsReset << "\n\r";
  return str1 == str2;
 }
  
/*----------------------------------------------------------------------------------------------------------------------*/
/* Checks if the given char must be removed or not. If yes it returns "true" and if no - returns "false".               */
bool CondToRemove(char c) /* Condition to remove. */
 {
  return c == ' ' || c == '_' || c == '-' || c == '\'' || c == '\"' || c == '`' || c == ';' || c == ':' || c == '.' || c == ',';
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Removes unnecessary characters from the string. that means from the name of a city before comparing.                 */
void RemoveUnneededChars(std::string &StringToClear)
 {
  StringToClear.erase(std::remove_if(StringToClear.begin(), StringToClear.end(), [](char c){return CondToRemove(c);}), StringToClear.end());
 }


/*----------------------------------------------------------------------------------------------------------------------*/
/* Checking the possible Name alternatives and adjusts the name to the defined standard before comparing.               */
void ReplaceSubStrings(std::string &StringToCorrect)
 {
  size_t i, j;
  size_t pos;
  std::string LastStringToCorrect;

  LastStringToCorrect = StringToCorrect;

  for(i = 0; i < NUM_ALT_NAME_BLOCKS; i++)
   {
    for(j = 1; (j < ALT_NAME_BLOCK_SIZE) && (AltNamesWords[i][j] != ""); j++)
     {
      if((pos = StringToCorrect.find(AltNamesWords[i][j])) != std::string::npos)
       {
        StringToCorrect.replace(pos, AltNamesWords[i][j].length(), AltNamesWords[i][0]);
        break;
       }
     }

   }
 }


/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Parking session Functions / Procedures 
 * *************************************************************************************************************
 */


/*----------------------------------------------------------------------------------------------------------------------*/
/* The session parking task procedure. Runs untill the client is connected and calculates praking price.                */
void HandleClient(int ClientSocket, uint16_t NumPriceDBCities, DBShmemPriceData_c **DBShmemPriceData, std::string ShapeFileName, Process_c *NetCl, std::string AddrStamp) 
 {
  bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */

  char buffer[BUFFER_SIZE];
  Customer_s CustomerInfo;
  CustAcknowledge_s CustAckInfo;  /* Customer Acknowledge Information. */
  uint8_t *DataForSending;
  ssize_t AckDataSize;            /* Acknowledge Data Size. */
  bool DecodeResult;
  time_t CurrentTime;
  bool FirstInt = true;           /* First repeat interration. */
  bool CityDetected = false;
  bool DataBaseChecked = false;
  bool CityFoundInDataBase = false;
  std::string DetectedCityName = "";
  uint32_t RegionCode = 0, EdRegCode = 0;
  std::string BufForMess;
  std::ostringstream stream;
  LogMessType_s MessageToLog;

  PriceTab_s CityPriceInfo = { 0, 0, 0, {0} };
  size_t i;
  uint16_t CityPPH = 0; /* City parking Price Per Hour. */
  std::string ProcName = PARK_PROC_NAME; // NetCl->GetProcName();
  
  std::cout << "Handling client in thread ID: " << std::this_thread::get_id() << "  Netrowk address: " << AddrStamp << "\n\r";
  CustAckInfo.ParkingDurationTime = 0;
  time(&CustAckInfo.ParkingStartTime);
  
  /* Communication loop. */
  while (true) 
   {
    memset(buffer, 0, BUFFER_SIZE);
    int BytesRead = read(ClientSocket, buffer, BUFFER_SIZE);
    
    if (BytesRead <= 0) 
     {
      std::cout << (StdOutNoPiping ? ResultColors[E_PROBLEM] : "") << "Client disconnected or error." << (StdOutNoPiping ? TermColorsReset : "") << "\n\r";
      if(NetCl != nullptr)
       {
        stream << "The cliet disconnected. Socket: " << ClientSocket << " Address: " << AddrStamp;
        BufForMess = stream.str();
        MessageToLog = MakeLogMessage(E_LOG_ATTENTION, ProcName.c_str(), BufForMess.c_str());
        NetCl->LogEvent(MessageToLog);
       }
      break;
     }

    std::cout << "Received " << BytesRead << " Bytes\n\r";

    DecodeResult = DecodeNetData((uint8_t*)buffer, BytesRead, (uint8_t *)&CustomerInfo);

    if(DecodeResult)
     {
      char NameBuf[75], CordsBuf[200];
      CreateCordsFormatted(CordsBuf, sizeof(CordsBuf), CustomerInfo.Cords, StdOutNoPiping);
      CreateNameFormated(NameBuf, sizeof(NameBuf), CustomerInfo.Customer_Name, StdOutNoPiping);
      CreateVehIDFormated(buffer, sizeof(buffer), CustomerInfo.Vechicle_ID, StdOutNoPiping);
      std::cout << "The customer is: " << NameBuf << " on the vehicle: " << buffer << (StdOutNoPiping ? TermColorsReset : "") << " In coordinates: " << CordsBuf << (StdOutNoPiping ? TermColorsReset : "") << "\n\r";

      if(FirstInt)
       {
        stream.str("");
        stream.clear();
        CreateVehIDFormated(buffer, sizeof(buffer), CustomerInfo.Vechicle_ID, false);
        stream << AddrStamp << ": The new customer " << CustomerInfo.Customer_Name << " on vehicle " << buffer; 
        CordsToString(buffer, sizeof(buffer), CustomerInfo.Cords);
        stream << " started parking. Coordinates: " << buffer;
        BufForMess = stream.str();
        MessageToLog = MakeLogMessage(E_LOG_MESSAGE, ProcName.c_str(), BufForMess.c_str());
        NetCl->LogEvent(MessageToLog);
        if(!CityDetected)   // City is not checked.
         {
          DetectedCityName = "Not Detected";
          std::cout << "Trying to detect the city..." << "\n\r";
          // The "ShapeFileName" should be got by the function "TaskControl_ShSM_c::GetSHPFileName()".    
          
          bool CityFound = DetectCity(CustomerInfo.Cords, DetectedCityName, RegionCode, EdRegCode,  ShapeFileName);

          stream.str("");
          stream.clear();

          if(CityFound)
           {
            std::cout << (StdOutNoPiping ? ResultColors[E_CORRECT] : "") << "City was detected: " << (StdOutNoPiping ? CITYNAME_COLOR : "") << DetectedCityName << (StdOutNoPiping ? TermColorsReset : "") <<"\n\r";
            stream << AddrStamp << ": City was detected: " << DetectedCityName;
            BufForMess = stream.str();
            MessageToLog = MakeLogMessage(E_LOG_MESSAGE, ProcName.c_str(), BufForMess.c_str());
            NetCl->LogEvent(MessageToLog);
            CityDetected = true;
           }
          else
           {
            std::cerr << (StdErrNoPiping ? ResultColors[E_FAIL] : "") << "City wasn\'t detected" << (StdErrNoPiping ? TermColorsReset : "") << "\n\r";
            MessageToLog = MakeLogMessage(E_LOG_ATTENTION, ProcName.c_str(), "City wasn\'t detected. Will be loaded zero price.");
            NetCl->LogEvent(MessageToLog);
            FirstInt = false;
           }
          strncpy(CityPriceInfo.City_Name, DetectedCityName.c_str(), sizeof(CityPriceInfo.City_Name) - 1);
         }
        if(CityDetected)  /* City is allready checked. Now it is possible to serach it in database.  Attention !!! This condition mustn't be written via else because the database checking must be done immediately. */
         {
          stream.str("");
          stream.clear();

          if(!DataBaseChecked)  /* DataBase isn't checked. */
           {
            if(DBShmemPriceData != nullptr) /* DataBase is given (Parameter of database eixists). */
             {
              if(*DBShmemPriceData != nullptr) /* The DataBase is loaded. */
               {
                for(i = 0; i < NumPriceDBCities; ++i)
                 {
                  (*DBShmemPriceData)->GetCity(i, &CityPriceInfo);
                  
                  if(StringsAreEqual(DetectedCityName, CityPriceInfo.City_Name))
                   {
                    std::ios old_state(nullptr);
                    old_state.copyfmt(std::cout); 
                    char old_fill = std::cout.fill();
      
                    CityPPH = CityPriceInfo.Price;
                    ConvertPrice(CityPriceInfo.Price, buffer, sizeof(buffer), E_PPH_FORMAT, StdOutNoPiping);
                    std::cout << (StdOutNoPiping ? ResultColors[E_CORRECT] : "") << "New parking detected in the city: " << (StdOutNoPiping ? CITYNAME_COLOR : "") << CityPriceInfo.City_Name << (StdOutNoPiping ? ResultColors[E_CORRECT] : "") << " ID: " << CityPriceInfo.City_ID << " Parking Price " << buffer << "\n\r";
                    std::cout.copyfmt(old_state);
                    std::cout.fill(old_fill);

                    stream << AddrStamp << ": The city " << DetectedCityName << " was found in the database and detected as " << CityPriceInfo.City_Name << "."; 
                    ConvertPrice(CityPriceInfo.Price, buffer, sizeof(buffer), E_PPH_FORMAT, false);
                    stream <<" The price will be " << buffer << ".";
                    BufForMess = stream.str();
                    MessageToLog = MakeLogMessage(E_LOG_MESSAGE, ProcName.c_str(), BufForMess.c_str());
                    NetCl->LogEvent(MessageToLog);

                    CityFoundInDataBase = true;
                    break;
                   }
                 }
                if(i >= NumPriceDBCities) /* The city wasn't found. In database. */
                 {
                  std::cerr << (StdErrNoPiping ? ResultColors[E_FAIL] : "") << "The City: " << (StdErrNoPiping ? CITYNAME_COLOR : "") << DetectedCityName << (StdErrNoPiping ? ResultColors[E_FAIL] : "") << " wasn't found in the database." << (StdErrNoPiping ? TermColorsReset : "") << "\n\r";
                  
                  stream << AddrStamp << "The city " << DetectedCityName << " was not found in database."; 
                  stream << " The price will be zero.";
                  BufForMess = stream.str();
                  MessageToLog = MakeLogMessage(E_LOG_WARNING, ProcName.c_str(), BufForMess.c_str());
                  NetCl->LogEvent(MessageToLog);

                  strncpy(CityPriceInfo.City_Name, DetectedCityName.c_str(), sizeof(CityPriceInfo.City_Name) - 1);
                 }
                FirstInt = false;
               }
              else /* The DataBase is still not loaded. */
               {
                std::cerr << (StdErrNoPiping ? TermRed : "") << "DataBase error." << (StdErrNoPiping ? TermColorsReset : "") << "  The database is not loaded." << "\n\r";
                FirstInt = false;
               }
              if((!CityFoundInDataBase) || (*DBShmemPriceData == nullptr)) // City in database doesn't exist.
               {
                std::cout << "Loading zero price.\n\r";
               }
              DataBaseChecked = true; 
             }
            else  /* The database is not given (Thea pointer to database is null). */
             {
              std::cerr << (StdErrNoPiping ? TermRed : "") << "DataBase error." << (StdErrNoPiping ? TermColorsReset : "") << "  The database is not given." << "\n\r";
              FirstInt = false;
             }
           }
         }

        if(!FirstInt)  /* All required data was acquired and now the result can be logged. */
         {
          stream.str("");
          stream.clear();
          CreateVehIDFormated(buffer, sizeof(buffer), CustomerInfo.Vechicle_ID, false);
          stream << AddrStamp << ": The start parking of the customer " << CustomerInfo.Customer_Name << " on vehicle " << buffer << " was registered.";
          CordsToString(buffer, sizeof(buffer), CustomerInfo.Cords);
          stream << " Coordinates: " << buffer << " City: " << CityPriceInfo.City_Name; 
          ConvertPrice(CityPriceInfo.Price, buffer, sizeof(buffer), E_PPH_FORMAT, false);
          stream << " price: " << buffer;
          ConvertTime(&CustAckInfo.ParkingStartTime, buffer, sizeof(buffer), E_CAL_FORMAT);
          stream << " Registered parking time: " << buffer;
          BufForMess = stream.str();
          MessageToLog = MakeLogMessage(E_LOG_MESSAGE, ProcName.c_str(), BufForMess.c_str());
          NetCl->LogEvent(MessageToLog);
         }
       }

      /* Loading info for response. */
      strncpy(CustAckInfo.City_Name, CityPriceInfo.City_Name, sizeof(CustAckInfo.City_Name));
      time(&CurrentTime);
      CustAckInfo.City_ID = CityPriceInfo.City_ID;
      CustAckInfo.OSM_ID = RegionCode;
      CustAckInfo.ParkingDurationTime = CurrentTime - CustAckInfo.ParkingStartTime;
      CustAckInfo.Vechicle_ID = CustomerInfo.Vechicle_ID;
      CustAckInfo.AccumulatedPrice = DIV_RND(CityPPH * CustAckInfo.ParkingDurationTime, 3600); /* Making diviation with rounding without using real (float or double) numbers. */
      CustAckInfo.ParkingEndTime = CurrentTime;
      CustAckInfo.PricePerHour = CityPPH;

      /* Enqueuing response to the database. */
      if((DBShmemPriceData != nullptr) && (*DBShmemPriceData != nullptr))  /* Attention !!! The condition cannot be changed places. The second condition can be and checked only if the first condition is true and only in this case the second condition will be checked due to shortcyrcuit method. */
       (*DBShmemPriceData)->SndClientParkingInfo(&CustomerInfo, &CustAckInfo);
    }
    else
     {
      std::cerr << (StdErrNoPiping ? TermRed : "") << "Error in decoding" << (StdErrNoPiping ? TermColorsReset : "") << "\n\r";
      CustAckInfo.City_ID = 1;
      strncpy(CustAckInfo.City_Name, "  -----  ", sizeof(CustAckInfo.City_Name) - 1);
      CustAckInfo.ParkingDurationTime = 0;
      CustAckInfo.Vechicle_ID = 0;
     }

    ssize_t WriteResult;
    /* Sending Response. */
    AckDataSize = EncodeNetData((uint8_t*)&CustAckInfo, sizeof(CustAckInfo), &DataForSending);
    WriteResult = write(ClientSocket, DataForSending, AckDataSize);
    if(WriteResult == -1)
     std::cerr << (StdErrNoPiping ? ResultColors[E_FAIL] : "") << "Error in sending respose." << (StdErrNoPiping ? TermColorsReset : "") << "\n\r";
    FreeData(&DataForSending);

   }
  

  stream.str("");
  stream.clear();
  CreateVehIDFormated(buffer, sizeof(buffer), CustomerInfo.Vechicle_ID, false);
  stream << AddrStamp << ": The parking was ended. Customer name: " << CustomerInfo.Customer_Name << " on vehicle " << buffer << ".";
  CordsToString(buffer, sizeof(buffer), CustomerInfo.Cords);
  stream << " Coordinates: " << buffer << ". City: " << CityPriceInfo.City_Name << "."; 
  ConvertPrice(CityPriceInfo.Price, buffer, sizeof(buffer), E_PPH_FORMAT, false);
  stream << " Price: " << buffer << ".";

  ConvertTime(&CustAckInfo.ParkingStartTime, buffer, sizeof(buffer), E_CAL_FORMAT);
  stream << " Start parking time: " << buffer;
  ConvertTime(&CurrentTime, buffer, sizeof(buffer), E_CAL_FORMAT);
  stream << " End parking time: " << buffer;
  ConvertTime(&CustAckInfo.ParkingDurationTime, buffer, sizeof(buffer), E_DUR_FORMAT);
  stream << " Parking duration: " << buffer;
  ConvertPrice(CustAckInfo.AccumulatedPrice, buffer, sizeof(buffer), E_ACC_FORMAT, false);
  stream << " Final price: " << buffer << ".";

  BufForMess = stream.str();
  MessageToLog = MakeLogMessage(E_LOG_MESSAGE, ProcName.c_str(), BufForMess.c_str());
  NetCl->LogEvent(MessageToLog);

  close(ClientSocket);
 }





/*======================================================================================================================*/