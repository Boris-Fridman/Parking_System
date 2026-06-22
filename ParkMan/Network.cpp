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






#define NETW_PROC_NAME     (char *)"Network"     /* Network process name*/

class Network_c: public Process_c
 {
    key_t LastPriceShmKey = 0;
    DBShmemPriceData_c *DBShmemPriceData = NULL;
    int serverSocket = 0, newSocket = 0;  
    sockaddr_in address = {{0},0,{0},{0}};
    int addrlen = 0;
  public:
    Network_c(char ProcName[], key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType);
    virtual ~Network_c();
    virtual void OnRunProcess();
    Network_c& operator = (const Network_c &other) = delete;
    Network_c(const Network_c &other) = delete;
 };




void NetworkProc(key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType)
 {
  Network_c Netw_Process(NETW_PROC_NAME, sh_mem_key, sem_name, ProcType);
  Netw_Process.RunProcess();
 }


// void CalculateParkingPriceTime(Customer_s *CustomerInfo, CustAcknowledge_s CustAckInfo)
//  {
  

//  }

void HandleClient(int clientSocket, uint16_t NumPriceDBCities = 0, DBShmemPriceData_c *DBShmemPriceData = NULL) 
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
  //char timebuf[100];
  bool FirstInt = true; /* First repeat interration. */
  std::string DetectedCityName = "";
  PriceTab_s CityPriceInfo = { 0, 0, 0, {0} };
  size_t i;
  uint16_t CityPPH = 0; /* City parking Price Per Hour. */
  
  std::cout << "Handling client in thread ID: " << std::this_thread::get_id() << "\n\r";
  CustAckInfo.ParkingDurationTime = 0;
  time(&CustAckInfo.ParkingStartTime);
  //ConvertTime(&CustAckInfo.ParkingStartTime, timebuf,sizeof(timebuf));

  /* Communication loop. */
  while (true) 
   {
    memset(buffer, 0, BUFFER_SIZE);
    int BytesRead = read(clientSocket, buffer, BUFFER_SIZE);
    
    if (BytesRead <= 0) 
     {
      std::cout << (StdOutNoPiping ? TermRed : "") << "Client disconnected or error." << (StdOutNoPiping ? TermColorsReset : "") << "\n\r";

      break;
     }

    std::cout << "Received " << BytesRead << " Bytes\n\r";

    DecodeResult = DecodeNetData((uint8_t*)buffer, BytesRead, (uint8_t *)&CustomerInfo);

    if(DecodeResult)
     {
      //std::cout << "The customer is: " << CustomerInfo.Name << " on the vehicle: " << (StdOutNoPiping ? TermBGYello TermBlack : "") << CustomerInfo.Vechicle_ID << (StdOutNoPiping ? TermColorsReset : "") << " In coordinates: ";
      //VehicleIDToString(buffer, sizeof(buffer), CustomerInfo.Vechicle_ID);
      //std::cout << "The customer is: " << CustomerInfo.Name << " on the vehicle: " << (StdOutNoPiping ? TermBGYello TermBlack : "") << buffer << (StdOutNoPiping ? TermColorsReset : "") << " In coordinates: ";
      CreateVechIDFormated(buffer, sizeof(buffer), CustomerInfo.Vechicle_ID, StdOutNoPiping);
      std::cout << "The customer is: " << CustomerInfo.Name << " on the vehicle: " << buffer << (StdOutNoPiping ? TermColorsReset : "") << " In coordinates: ";
      PrintGPSCords(CustomerInfo.Cords);
      std::cout << (StdOutNoPiping ? TermColorsReset : "") << "\n\r";

      //DetectedCityName = "Tel Aviv";
      DetectedCityName = "Givataim";

      if(FirstInt)
       {
        if(DBShmemPriceData != NULL)
         {
          for(i = 0; i < NumPriceDBCities; ++i)
           {
            DBShmemPriceData->GetCity(i, &CityPriceInfo);
            if(strcmp(DetectedCityName.c_str(), CityPriceInfo.City_Name) == 0)
             {
              std::ios old_state(nullptr);
              old_state.copyfmt(std::cout); 
              char old_fill = std::cout.fill();

              CityPPH = CityPriceInfo.Price;
              std::cout << (StdOutNoPiping ? TermGreen : "") << "New parking detected in the city: " << CityPriceInfo.City_Name << " ID: " << CityPriceInfo.City_ID << " Parking Price " << CityPriceInfo.Price / 100<< "." << std::setfill('0') << std::setw(2) << CityPriceInfo.Price % 100 << "₪/h" << (StdOutNoPiping ? TermColorsReset : "") << "\n\r";

              std::cout.copyfmt(old_state);
              std::cout.fill(old_fill);

              break;
             }
           }
          FirstInt = false;
         }
       }

      
      /* Loading info for response. */
      strcpy(CustAckInfo.City_Name, DetectedCityName.c_str());
      time(&CurrentTime);
      CustAckInfo.City_ID = CityPriceInfo.City_ID;
      CustAckInfo.ParkingDurationTime = CurrentTime - CustAckInfo.ParkingStartTime;
      CustAckInfo.Vechicle_ID = CustomerInfo.Vechicle_ID;
      CustAckInfo.AccumulatedPrice = DIV_RND(CityPPH * CustAckInfo.ParkingDurationTime, 3600); /* Making diviation with rounding without using real (float or double) numbers. */
     }
    else
     {
      std::cerr << (StdErrNoPiping ? TermRed : "") << "Error in decoding" << (StdErrNoPiping ? TermColorsReset : "") << "\n\r";
      CustAckInfo.City_ID = 1;
      strcpy(CustAckInfo.City_Name, "  -----  ");
      CustAckInfo.ParkingDurationTime = 0;
      CustAckInfo.Vechicle_ID = 0;
     }
    /* Sending Response. */
    AckDataSize = EncodeNetData((uint8_t*)&CustAckInfo, sizeof(CustAckInfo), &DataForSending);
    write(clientSocket, DataForSending, AckDataSize);
    FreeData(&DataForSending);
   }
  
  close(clientSocket);
 }




Network_c::Network_c(char ProcName[], key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType)
 :Process_c(ProcName, sh_mem_key, sem_name, ProcType)
 {
  bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */

  int opt = 1;
  timeval timeout;

  timeout.tv_sec = 1;
  timeout.tv_usec = 0;

  addrlen = sizeof(address);

  /* Create the server socket */
  if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
   {
    //std::cerr << "Socket creation failed\n\r";
    perr() << (StdErrNoPiping ? ResultColors[E_FAIL] : "") << "Socket creation failed" << (StdErrNoPiping ? TermColorsReset : "");
    return;
   }

  /* Attach socket to the port (prevents 'Address already in use' errors) */
  if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) 
   {
    perr() << (StdErrNoPiping ? ResultColors[E_FAIL] : "") << "setsockopt failed" << (StdErrNoPiping ? TermColorsReset : "");
    MakeExit();
    return;
   }


  if (setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) 
   {
    //std::cerr << "Setting SO_RCVTIMEO failed\n\r";
    perr() << (StdErrNoPiping ? ResultColors[E_FAIL] : "") << "Setting SO_RCVTIMEO failed" << (StdErrNoPiping ? TermColorsReset : "");
    // Handle error or close socket
   }


  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(DESTIN_PORT);

  /* Bind the socket */
  if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0) 
   {
    //std::cerr << "Bind failed\n\r";
    perr() << (StdErrNoPiping ? ResultColors[E_FAIL] : "") << "Bind failed" << (StdErrNoPiping ? TermColorsReset : "");
    MakeExit();
    return;
   }

  /* Start listening for incoming connections */
  if (listen(serverSocket, 3) < 0) 
   {
    //std::cerr << "Listen failed\n\r";
    perr() << (StdErrNoPiping ? ResultColors[E_FAIL] : "") << "Listen failed" << (StdErrNoPiping ? TermColorsReset : "");
    MakeExit();
    return;
   }
  std::cout << (StdOutNoPiping ? ResultColors[E_CORRECT] : "") << "Server listening on port " << DESTIN_PORT << "..." << (StdOutNoPiping ? TermColorsReset : "") << "\n\r";
 }

void Network_c::OnRunProcess()
 {
  bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */

  if((LastPriceShmKey != ((ControlDBPrice_s*)p_shm)->CitiesNewShmKey) && (((ControlDBPrice_s*)p_shm)->CitiesNewShmKey) != 0) /* Checking if the cities' prices' database memory was loaded or changed. */
   {
    if(DBShmemPriceData != NULL)
     DBShmemPriceData->ReallocateShmem(((ControlDBPrice_s*)p_shm)->NumPriceDBCities, ((ControlDBPrice_s*)p_shm)->CitiesNewShmKey);
    else
     DBShmemPriceData = new DBShmemPriceData_c(((ControlDBPrice_s*)p_shm)->CitiesNewShmKey, ((ControlDBPrice_s*)p_shm)->CitiesSemName, ((ControlDBPrice_s*)p_shm)->NumPriceDBCities);
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
     }

    std::cout << (StdOutNoPiping ? ResultColors[E_CORRECT] : "") << "New connection accepted." << (StdOutNoPiping ? TermColorsReset : "") << "\n\r";
    /* Spawn a new thread to handle the client */
    std::thread t(HandleClient, newSocket, ((ControlDBPrice_s*)p_shm)->NumPriceDBCities, DBShmemPriceData);
    t.detach(); /* Detach the thread so it runs independently */
   }
  //std::cout << "Test Delay\n\r";
 }
 
Network_c::~Network_c()
 {
  if(serverSocket != 0)
   {
    close(serverSocket);
    serverSocket = 0;
    std::cout << "The socket was closed. \n\r";
   }
  if(DBShmemPriceData == NULL)
   delete DBShmemPriceData;
 }


