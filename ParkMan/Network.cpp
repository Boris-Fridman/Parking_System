
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
