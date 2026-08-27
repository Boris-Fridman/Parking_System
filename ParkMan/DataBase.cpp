
/*======================================================================================================================*/

#include "DataBase.hpp"
#include "main.hpp"
#include "Errors.hpp"
#include "Processes.hpp"
#include "PriceDataBase.h"

#include <semaphore.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sqlite3.h>

#include "Logging.h"

#define QUEUE_NAME     "/park_pr_db_q"   /* Attention !!!  The length mustn't exceed the strlen("NAME_LEN") - 12 definition size because in some stractures this name is stored in limited-length-char-array and to the end of this name is added a 10-digit number. */ //"/parkprice" //"/park_price"  //"/park_price_database_queue"


/*======================================================================================================================*/

class DataBase_c: public Process_c
 {
    DBShmemPriceData_c *DBShmemPriceData = nullptr;
    sqlite3 *conn = nullptr;
  public:
    DataBase_c(char ProcName[], key_t sh_mem_key, const char sem_name[], std::string sq_name, std::string qsem_name, ProcTypeID_e ProcType);
    virtual ~DataBase_c();
  protected:
    virtual void OnStartProcess();
    virtual void OnRunProcess();
    virtual void OnFinishProcess();
  public:
    DataBase_c& operator = (const DataBase_c &other) = delete;
    DataBase_c(const DataBase_c &other) = delete;
  protected:
    void LoadDataBase();
 };

 //ProcParams_s Procparams
void DataBaseProc(key_t sh_mem_key, const char sem_name[], std::string sq_name, std::string qsem_name, ProcTypeID_e ProcType)
 {
  DataBase_c DB_Process(DB_PROC_NAME, sh_mem_key, sem_name, sq_name, qsem_name, ProcType);
  DB_Process.RunProcess();
 }





void DataBase_c::OnStartProcess()
 {
  LogMessType_s MessageToLog;

  DBShmemPriceData = new DBShmemPriceData_c(0, this);

  MessageToLog = MakeLogMessage(E_LOG_MESSAGE, GetProcName().c_str(), "Loading database with cities... "); 
  LogEvent(MessageToLog);      

  LoadDataBase();
 }

void DataBase_c::OnRunProcess()
 {
  Process_c::OnRunProcess();

  LogMessType_s MessageToLog;
  
  if(DataBaseMustBeReloaded())
   {
    MessageToLog = MakeLogMessage(E_LOG_MESSAGE, GetProcName().c_str(), "Updateing database with cities... "); 
    LogEvent(MessageToLog);      

    LoadDataBase();
   }

  if(DBShmemPriceData != nullptr)
   DBShmemPriceData->CheckMessageExistance(&conn);
  
 };

void DataBase_c::OnFinishProcess()
 {
  delete DBShmemPriceData;
 }


DataBase_c::DataBase_c(char ProcName[], key_t sh_mem_key, const char sem_name[], std::string sq_name, std::string qsem_name, ProcTypeID_e ProcType)
 :Process_c(ProcName, sh_mem_key, sem_name, sq_name, qsem_name, ProcType)
 {
  
 }

DataBase_c::~DataBase_c()
 {

 }






void DataBase_c::LoadDataBase()
 {
  std::string DBFileName;
  int result;
  PriceTab_s *ListOfCities = nullptr;
  int NumCities;

  std::string BufForMess;
  std::ostringstream stream;
  LogMessType_s MessageToLog;

  stream.str("");
  stream.clear();

  MessageToLog = MakeLogMessage(E_LOG_MESSAGE, GetProcName().c_str(), "Loading cities from database ... "); 
  LogEvent(MessageToLog);      
  
  DBFileName = GetDBFileName();
  SetDBPathName(DBFileName.c_str()); /* Loading name of the file contains the database of prices with the parking systems. */
  result = GetCitiesList(&conn, &ListOfCities, &NumCities);
  if((result == 0) && (NumCities > 0))  /* Database contains data. */
   {
    DBShmemPriceData->LoadCitiesList(ListOfCities, NumCities);
    ((ControlDBPrice_s*)p_shm)->NumPriceDBCities = NumCities;
    ((ControlDBPrice_s*)p_shm)->CitiesNewShmKey = DBShmemPriceData->ShMemKey();
    strncpy( ((ControlDBPrice_s*)p_shm)->CitiesSemName , DBShmemPriceData->SemName().c_str() , NAME_LEN - 1 );
    //((ControlDBPrice_s*)p_shm)->DBUpdated = true;
    ((ControlDBPrice_s*)p_shm)->DBUpdateRequired = false;
    stream << NumCities << " cities were loaded.";
    BufForMess = stream.str();
    MessageToLog = MakeLogMessage(E_LOG_EVENT, GetProcName().c_str(), BufForMess.c_str());
    LogEvent(MessageToLog);
   }
  else
   {
    MessageToLog = MakeLogMessage(E_LOG_ATTENTION, GetProcName().c_str(), "The databse didn't contain any city.");
    LogEvent(MessageToLog);
   }

  /* The shared queue and its semaphore must be loaded anyway independently if the shared memory exists or not due to no city exist in database. */
  strncpy( ((ControlDBPrice_s*)p_shm)->ReportQueueName , DBShmemPriceData->QueueName().c_str(), NAME_LEN - 1);
  strncpy( ((ControlDBPrice_s*)p_shm)->ReportSemName   , DBShmemPriceData->QSemName().c_str(), NAME_LEN - 1);

  /* Freeing temporary-loaded-list of cityes after it was received and loaded to shared memory. If it was received as empty it will be checked inside the function "FreeList()" before clearing. */
  FreeList(&ListOfCities);  /* No need to compare the list to nullptr because it is compared in the procedure itself. Even more it should be run anyway without any condition to prevent emergency memory leakage. */

  MessageToLog = MakeLogMessage(E_LOG_EVENT, GetProcName().c_str(), "The database was loaded. "); 
  LogEvent(MessageToLog);      

 }

void DBShmemPriceData_c::CheckMessageExistance(sqlite3 **conn)
 {
  // bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  unsigned int prio;
  ClientQueueMsg_s ClientQueueMsg;  /* Customer Acknowledge Information. */
  struct timespec ts;
  char buffer[50];

  if (clock_gettime(CLOCK_REALTIME, &ts) != -1) 
   {
    ++ts.tv_sec;
   }
  
  ssize_t bytes_read = mq_timedreceive(p_sq, (char*)&ClientQueueMsg, sizeof(ClientQueueMsg_s), &prio, &ts);
  //std::cout << "Num queue received bytes " << bytes_read << "\n\r";
  if(bytes_read >= 0)
   {
    ConvertPrice(ClientQueueMsg.AccumulatedPrice, buffer, sizeof(buffer), E_ACC_FORMAT, StdOutNoPiping);
    std::cout << "City: " << (StdOutNoPiping ? CITYNAME_COLOR : "") << ClientQueueMsg.City_Name << (StdOutNoPiping ? TermColorsReset : "") << "    Price: " << buffer << (StdOutNoPiping ? TermColorsReset : "") << "\n\r";
    AddOrUpdateParkingSession(conn, ClientQueueMsg);
   }
 }


void DBShmemPriceData_c::AddOrUpdateParkingSession(sqlite3 **conn, ClientQueueMsg_s &ClientQueueMsg)
 {
  bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */

  int result = 0;
  char timedurbuf[50], vehidbuf[50], pricebuf[50];

  std::string BufForMess;
  std::ostringstream stream;
  LogMessType_s MessageToLog;


  CreateVehIDFormated(vehidbuf, sizeof(vehidbuf), ClientQueueMsg.Vechicle_ID, StdOutNoPiping);
  CreateLoadDatabase(conn); // Yes, the given pointer to database must be given as pointer to pointer to database because it's address is updated in this function.
  result = UpdateParkSessionInDataBase(conn, ClientQueueMsg);
  if(result == 0)  /* The reqauired parking session allready exists in database. */
   {
    std::ios old_state(nullptr);
    old_state.copyfmt(std::cout); 
    char old_fill = std::cout.fill();

    
    //ConvertPrice(ClientQueueMsg.AccumulatedPrice, pricebuf, sizeof(pricebuf), E_ACC_FORMAT, StdOutNoPiping);
    std::cout << (StdOutNoPiping ? ResultColors[E_CORRECT] : "") << "The parking session of "<< ClientQueueMsg.Customer_Name << " " << vehidbuf << (StdOutNoPiping ? ResultColors[E_CORRECT] : "") << " was already existing. Were updated only time and price." << (StdOutNoPiping ? TermColorsReset : "") << "\n\r";
    ConvertPrice(ClientQueueMsg.AccumulatedPrice, pricebuf, sizeof(pricebuf), E_ACC_FORMAT, StdOutNoPiping);
    std::cout << (StdOutNoPiping ? ResultColors[E_CORRECT] : "") << "The parking price was updated to: " << pricebuf << "." << (StdOutNoPiping ? TermColorsReset : "") << "\n\r";

    std::cout.copyfmt(old_state);
    std::cout.fill(old_fill);

   }
  else if(result < 0)  /* The update wasn't be possible because the parking session didn't exist. */
   {
    result =  WriteNewParkSessionToDataBase(conn, ClientQueueMsg);
    if(result == 0)
     {
      ConvertTime(&ClientQueueMsg.ParkingStartTime, timedurbuf, sizeof(timedurbuf), E_CAL_FORMAT);

      std::cout << (StdOutNoPiping ? ResultColors[E_CORRECT] : "") << "The parking session of " << ClientQueueMsg.Customer_Name << " " << vehidbuf << (StdOutNoPiping ? ResultColors[E_CORRECT] : "") << " starting at " << timedurbuf << " was added successfully. "<< (StdOutNoPiping ? TermColorsReset : "") <<"\n\r";

      if(DbsCl != nullptr)
       {
        CreateVehIDFormated(vehidbuf, sizeof(vehidbuf), ClientQueueMsg.Vechicle_ID, false);
        stream << "The new parking session of the customer " << ClientQueueMsg.Customer_Name << " of the vehicle: " << vehidbuf << "started at " << timedurbuf << " was added to the database.";
        BufForMess = stream.str();
        MessageToLog = MakeLogMessage(E_LOG_EVENT, DbsCl->GetProcName().c_str(), BufForMess.c_str());
        DbsCl->LogEvent(MessageToLog);      
       }
     }
   }
  if(result == -1)
   {
    
    std::cerr << (StdErrNoPiping ? ResultColors[E_FAIL] : "") << "Error" << (StdErrNoPiping ? TermColorsReset : "") << "\n\r";

    // if(StdErrNoPiping)fprintf(stderr, "%s", ResultColors[E_FAIL]);
    // printf("Error\n\r");
    // if(StdErrNoPiping)fprintf(stderr, "%s", TermColorsReset);

   }

 }



/*======================================================================================================================*/



DBShmemPriceData_c::DBShmemPriceData_c(int NCities, Process_c *DbsCl_ToSet)
 :ShSemMemQue_c(NCities*sizeof(PriceTab_s), QUEUE_RECEIVE_E, QUEUE_NAME, sizeof(ClientQueueMsg_s)), DbsCl(DbsCl_ToSet)
 {
  // LoadShq(QUEUE_RECEIVE_E);
  // LoadShqs();
 }
  
DBShmemPriceData_c::DBShmemPriceData_c(key_t sh_mem_key, const char sem_name[], std::string sq_name, std::string qsem_name, uint16_t NCities, Process_c *DbsCl_ToSet)
 :ShSemMemQue_c(sh_mem_key, sem_name, sq_name, qsem_name, NCities * sizeof(PriceTab_s), QUEUE_SEND_E, sizeof(ClientQueueMsg_s)), DbsCl(DbsCl_ToSet)
 {
  // this->sq_name = sq_name;
  // this->qsem_name = qsem_name;
  // LoadShq(QUEUE_SEND_E);
  // LoadShqs();
 }
  
DBShmemPriceData_c::~DBShmemPriceData_c()
 {
  // RemoveShq();
  // RemoveShqs();
 }

void DBShmemPriceData_c::ReallocateShmem(uint16_t NewNumCities, key_t new_sh_mem_key)
 {
  if(((sh_mem_key != 0) && (sh_mem_key != new_sh_mem_key)) || (OldNumCities != NewNumCities))
   {
    RemoveShm();
    if(new_sh_mem_key > 0)
     sh_mem_key = new_sh_mem_key;
    LoadShm(NewNumCities * sizeof(PriceTab_s));
    OldNumCities = NewNumCities;
   }
 }

void DBShmemPriceData_c::LoadCitiesList(PriceTab_s ListOfCities[], int NumCities)
 {
  sem_wait(p_shs);
  ReallocateShmem(NumCities);
  memcpy(p_shm, ListOfCities, NumCities * sizeof(ListOfCities[0]));
  sem_post(p_shs);
 }

void DBShmemPriceData_c::GetCity(uint16_t CityNo, PriceTab_s *CityPriceInfo)
 {
  sem_wait(p_shs);
  *CityPriceInfo = ((PriceTab_s*)p_shm)[CityNo];
  sem_post(p_shs);

 }


void DBShmemPriceData_c::SndClientParkingInfo(Customer_s *CustomerInfo, CustAcknowledge_s *CustAckInfo)
 {
  ClientQueueMsg_s ClientMsg;
  size_t Len;

  ClientMsg.Cords                 =  CustomerInfo->Cords;
  strncpy(ClientMsg.Customer_Name ,  CustomerInfo->Customer_Name,      sizeof(ClientMsg.Customer_Name));
  ClientMsg.Vechicle_ID           =  CustomerInfo->Vechicle_ID;

  ClientMsg.ParkingStartTime      =  CustAckInfo->ParkingStartTime;
  ClientMsg.ParkingEndTime        =  CustAckInfo->ParkingEndTime;
  ClientMsg.ParkingDurationTime   =  CustAckInfo->ParkingDurationTime;
  ClientMsg.OSM_ID                =  CustAckInfo->OSM_ID;
  ClientMsg.City_ID               =  CustAckInfo->City_ID;
  strncpy(ClientMsg.City_Name     ,  CustAckInfo->City_Name,           sizeof(ClientMsg.City_Name));
  ClientMsg.AccumulatedPrice      =  CustAckInfo->AccumulatedPrice;
  ClientMsg.PricePerHour          =  CustAckInfo->PricePerHour;

  Len = sizeof(ClientQueueMsg_s);

//  std::cout << "Trying to take queue semaphore ... " << p_shqs << "\n\r";
  sem_wait(p_shqs);
//  std::cout << "The semaphore was taken successrully. \n\r";
  if (mq_send(p_sq, (char*)&ClientMsg, Len, 0) == -1)
   {
    perror("mq_send failed");
   } 
  else 
   {
    printf("Message sent successfully. %ld bytes sent.\n\r", Len);
   }
//  std::cout << "Giving the queue semaphore back ... \n\r";
  sem_post(p_shqs);
//  std::cout << "The semaphore was given successfully. \n\r";
 }




/*======================================================================================================================*/
