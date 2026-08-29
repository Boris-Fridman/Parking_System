
/*======================================================================================================================*/

#include "Network.hpp"
#include "main.hpp"
#include "Errors.hpp"
#include "Processes.hpp"
#include "DataBase.hpp"

#include <semaphore.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <thread>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Parking.hpp"

#include "Configuration.hpp"

/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          The class defining object controlling the Network Process.
 * *************************************************************************************************************
 */

class Network_c: public Process_c
 {
    key_t LastPriceShmKey = -1;  /* Setting to -1 instead of 0 to ensure that in the first time running when the PriceShmKey == 0. */
    DBShmemPriceData_c *DBShmemPriceData = nullptr;
    int serverSocket = 0, newSocket = 0;  
    sockaddr_in address = {{0},0,{0},{0}};
    int addrlen = 0;
  public:
    Network_c(char ProcName[], key_t sh_mem_key, const char sem_name[], std::string sq_name, std::string qsem_name, ProcTypeID_e ProcType);  /* Initilizing constructor of the Networ Object.                                                                        */
    virtual ~Network_c();                                                                                                                    /* Deinitilizing destructor of the Network Object.                                                                      */
  protected:
    virtual void OnRunProcess();                                                                                                             /* Checks the DB Shared memory state and opens the new tasks of the parking session on the new client connection.       */
  public:
    Network_c& operator = (const Network_c &other) = delete;
    Network_c(const Network_c &other) = delete;
 };


/*======================================================================================================================*/


/*----------------------------------------------------------------------------------------------------------------------*/
/* Main Process' operating procedure.                                                                                   */
void NetworkProc(key_t sh_mem_key, const char msem_name[], std::string sq_name, std::string qsem_name, ProcTypeID_e ProcType)
 {
  Network_c Netw_Process(NETW_PROC_NAME, sh_mem_key, msem_name, sq_name, qsem_name, ProcType);
  Netw_Process.RunProcess();
 }


/*----------------------------------------------------------------------------------------------------------------------*/
/* The session parking task procedure. Runs untill the client is connected and calculates praking price.                */
void HandleClient(int ClientSocket, uint16_t NumPriceDBCities = 0, DBShmemPriceData_c **DBShmemPriceData = nullptr, std::string ShapeFileName = "", Network_c *NetCl = nullptr, std::string AddrStamp = "        ---        ") 
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
        MessageToLog = MakeLogMessage(E_LOG_ATTENTION, NetCl->GetProcName().c_str(), BufForMess.c_str());
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
        MessageToLog = MakeLogMessage(E_LOG_MESSAGE, NetCl->GetProcName().c_str(), BufForMess.c_str());
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
            MessageToLog = MakeLogMessage(E_LOG_MESSAGE, NetCl->GetProcName().c_str(), BufForMess.c_str());
            NetCl->LogEvent(MessageToLog);
            CityDetected = true;
           }
          else
           {
            std::cerr << (StdErrNoPiping ? ResultColors[E_FAIL] : "") << "City wasn\'t detected" << (StdErrNoPiping ? TermColorsReset : "") << "\n\r";
            MessageToLog = MakeLogMessage(E_LOG_ATTENTION, NetCl->GetProcName().c_str(), "City wasn\'t detected. Will be loaded zero price.");
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
                    MessageToLog = MakeLogMessage(E_LOG_MESSAGE, NetCl->GetProcName().c_str(), BufForMess.c_str());
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
                  MessageToLog = MakeLogMessage(E_LOG_WARNING, NetCl->GetProcName().c_str(), BufForMess.c_str());
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
          MessageToLog = MakeLogMessage(E_LOG_MESSAGE, NetCl->GetProcName().c_str(), BufForMess.c_str());
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
  MessageToLog = MakeLogMessage(E_LOG_MESSAGE, NetCl->GetProcName().c_str(), BufForMess.c_str());
  NetCl->LogEvent(MessageToLog);

  close(ClientSocket);
 }


/*======================================================================================================================*/


/*----------------------------------------------------------------------------------------------------------------------*/
/* Initilizing constructor of the Networ Object.                                                                        */
Network_c::Network_c(char ProcName[], key_t sh_mem_key, const char sem_name[], std::string sq_name, std::string qsem_name, ProcTypeID_e ProcType)
 :Process_c(ProcName, sh_mem_key, sem_name, sq_name, qsem_name, ProcType)
 {
  bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */

  int opt = 1;
  uint16_t DestinPort;
  timeval timeout;
  std::string BufForMess;
  std::ostringstream stream;
  LogMessType_s MessageToLog;


  DestinPort = GetDestinPort();

  timeout.tv_sec = 1;
  timeout.tv_usec = 0;

  addrlen = sizeof(address);

  /* Create the server socket */
  if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
   {
    perr() << (StdErrNoPiping ? ResultColors[E_FAIL] : "") << "Socket creation failed" << (StdErrNoPiping ? TermColorsReset : "");
    MessageToLog = MakeLogMessage(E_LOG_FAIL, GetProcName().c_str(), std::strerror(errno));
    LogEvent(MessageToLog);      
    return;
   }
  else
   {
    stream << "The socket " << serverSocket << " was opened successfully. ";
    BufForMess = stream.str();
    std::cout << BufForMess << "\n\r";
    MessageToLog = MakeLogMessage(E_LOG_EVENT, GetProcName().c_str(), BufForMess.c_str());
    LogEvent(MessageToLog);      
    stream.str("");
    stream.clear();
   }

  /* Attach socket to the port (prevents 'Address already in use' errors) */
  if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) 
   {
    perr() << (StdErrNoPiping ? ResultColors[E_FAIL] : "") << "setsockopt failed" << (StdErrNoPiping ? TermColorsReset : "");
    MakeExit();
    MessageToLog = MakeLogMessage(E_LOG_FAIL, GetProcName().c_str(), std::strerror(errno));
    LogEvent(MessageToLog);      
    return;
   }


  if (setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) 
   {
    perr() << (StdErrNoPiping ? ResultColors[E_FAIL] : "") << "Setting SO_RCVTIMEO failed" << (StdErrNoPiping ? TermColorsReset : "");
    MessageToLog = MakeLogMessage(E_LOG_FAIL, GetProcName().c_str(), std::strerror(errno));
    LogEvent(MessageToLog);      
    /* Handle error or close socket. */
   }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(DestinPort);

  /* Bind the socket */
  if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0) 
   {
    perr() << (StdErrNoPiping ? ResultColors[E_FAIL] : "") << "Bind failed" << (StdErrNoPiping ? TermColorsReset : "");
    MessageToLog = MakeLogMessage(E_LOG_FAIL, GetProcName().c_str(), std::strerror(errno));
    LogEvent(MessageToLog);      
    MakeExit();
    return;
   }

  /* Start listening for incoming connections */
  if (listen(serverSocket, 3) < 0) 
   {
    perr() << (StdErrNoPiping ? ResultColors[E_FAIL] : "") << "Listen failed" << (StdErrNoPiping ? TermColorsReset : "");
    MessageToLog = MakeLogMessage(E_LOG_FAIL, GetProcName().c_str(), std::strerror(errno));
    LogEvent(MessageToLog);      
    MakeExit();
    return;
   }
  stream << "Server listening on port " << DestinPort << "...";
  BufForMess = stream.str();
  std::cout << (StdOutNoPiping ? ResultColors[E_CORRECT] : "") << BufForMess << (StdOutNoPiping ? TermColorsReset : "") << "\n\r";
  MessageToLog = MakeLogMessage(E_LOG_MESSAGE, GetProcName().c_str(), BufForMess.c_str());
  LogEvent(MessageToLog);      
 }

/*----------------------------------------------------------------------------------------------------------------------*/
/* Checks the DB Shared memory state and opens the new tasks of the parking session on the new client connection.       */
void Network_c::OnRunProcess()
 {
  bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */

  std::string BufForMess;
  std::ostringstream stream;
  char ip_string[INET_ADDRSTRLEN];
  LogMessType_s MessageToLog;


  if((DBShmemPriceData == nullptr) && (((ControlDBPrice_s*)p_shm)->ReportQueueName[0] != '\0') && (((ControlDBPrice_s*)p_shm)->ReportSemName[0] != '\0'))
   {
    DBShmemPriceData = new DBShmemPriceData_c(((ControlDBPrice_s*)p_shm)->CitiesNewShmKey, ((ControlDBPrice_s*)p_shm)->CitiesSemName, ((ControlDBPrice_s*)p_shm)->ReportQueueName, ((ControlDBPrice_s*)p_shm)->ReportSemName, ((ControlDBPrice_s*)p_shm)->NumPriceDBCities, this);
   }

  if((LastPriceShmKey != ((ControlDBPrice_s*)p_shm)->CitiesNewShmKey) /*&& (((ControlDBPrice_s*)p_shm)->CitiesNewShmKey != 0)*/) /* Checking if the cities' prices' database memory was loaded or changed. */
   {
    if(DBShmemPriceData != nullptr)
     DBShmemPriceData->ReallocateShmem(((ControlDBPrice_s*)p_shm)->NumPriceDBCities, ((ControlDBPrice_s*)p_shm)->CitiesNewShmKey);
    // else
    //  DBShmemPriceData = new DBShmemPriceData_c(((ControlDBPrice_s*)p_shm)->CitiesNewShmKey, ((ControlDBPrice_s*)p_shm)->CitiesSemName, ((ControlDBPrice_s*)p_shm)->ReportQueueName, ((ControlDBPrice_s*)p_shm)->ReportSemName, ((ControlDBPrice_s*)p_shm)->NumPriceDBCities);
    LastPriceShmKey = ((ControlDBPrice_s*)p_shm)->CitiesNewShmKey;
   }
  
  
  /* Accept connections in a loop */
  if ((newSocket = accept(serverSocket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) >= 0) 
   {
    timeval timeout;
  
    timeout.tv_sec = 20;
    timeout.tv_usec = 0;

    if (setsockopt(newSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) 
     {
      perr() << (StdErrNoPiping ? ResultColors[E_FAIL] : "") << "Setting SO_RCVTIMEO failed" << (StdErrNoPiping ? TermColorsReset : "");    

      MessageToLog = MakeLogMessage(E_LOG_FAIL, GetProcName().c_str(), std::strerror(errno));
      LogEvent(MessageToLog);      
     }
    else
     {
      std::cout << (StdOutNoPiping ? ResultColors[E_CORRECT] : "") << "New connection accepted." << (StdOutNoPiping ? TermColorsReset : "") << "\n\r";

      /* Spawn a new thread to handle the client */
      inet_ntop(AF_INET, &address.sin_addr, ip_string, sizeof(ip_string));
      stream << "The new client was connected: " << "Socket: " << newSocket << "  Address: " << ip_string << " port: " << address.sin_port;
      BufForMess = stream.str();
      
      std::cout << BufForMess << "\n\r";
      MessageToLog = MakeLogMessage(E_LOG_FAIL, GetProcName().c_str(), BufForMess.c_str());
      LogEvent(MessageToLog);      
      stream.str("");
      stream.clear();
      stream << ip_string << ":" << address.sin_port;
      std::thread t(HandleClient, newSocket, ((ControlDBPrice_s*)p_shm)->NumPriceDBCities, &DBShmemPriceData, GetSHPFileName(), this, stream.str());
      t.detach(); /* Detach the thread so it runs independently */
     }
   }
 }
 
/*----------------------------------------------------------------------------------------------------------------------*/
/* Deinitilizing destructor of the Network Object.                                                                      */
Network_c::~Network_c()
 {
  std::string BufForMess;
  std::ostringstream stream;
  LogMessType_s MessageToLog;
  if(serverSocket != 0)
   {
    stream << "The sockt " << serverSocket << " was closed.";
    close(serverSocket);
    serverSocket = 0;
    BufForMess = stream.str();
    std::cout << BufForMess << "\n\r";
    MessageToLog = MakeLogMessage(E_LOG_EVENT, GetProcName().c_str(), BufForMess.c_str());
    LogEvent(MessageToLog);      
   }
  if(DBShmemPriceData == nullptr)
   delete DBShmemPriceData;
 }


/*======================================================================================================================*/
