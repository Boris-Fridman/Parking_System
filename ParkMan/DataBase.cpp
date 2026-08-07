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

#define DB_PROC_NAME     (char *)"DataBase"     /* Database process name*/

#define QUEUE_NAME     "/park_pr_db_q"   /* Attention !!!  The length mustn't exceed the strlen("NAME_LEN") - 12 definition size because in some stractures this name is stored in limited-length-char-array and to the end of this name is added a 10-digit number. */ //"/parkprice" //"/park_price"  //"/park_price_database_queue"


/*======================================================================================================================*/

class DataBase_c: public Process_c
 {
    DBShmemPriceData_c *DBShmemPriceData = nullptr;
    sqlite3 *conn = nullptr;
  public:
    DataBase_c(char ProcName[], key_t sh_mem_key, const char sem_name[], std::string sq_name, std::string qsem_name, ProcTypeID_e ProcType);
    virtual ~DataBase_c();
    virtual void OnStartProcess();
    virtual void OnRunProcess();
    virtual void OnFinishProcess();
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
  DBShmemPriceData = new DBShmemPriceData_c(0);
  LoadDataBase();
 }

void DataBase_c::OnRunProcess()
 {
  Process_c::OnRunProcess();
  
  if(DataBaseMustBeReloaded())
   {
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
  
  DBFileName = GetDBFileName();
  SetDBPathName(DBFileName.c_str()); /* Loading name of the file contains the database of prices with the parking systems. */
  result = GetCitiesList(&conn, &ListOfCities, &NumCities);
  if((result == 0) && (NumCities > 0))
   {
    DBShmemPriceData->LoadCitiesList(ListOfCities, NumCities);
    ((ControlDBPrice_s*)p_shm)->NumPriceDBCities = NumCities;
    ((ControlDBPrice_s*)p_shm)->CitiesNewShmKey = DBShmemPriceData->ShMemKey();
    strncpy( ((ControlDBPrice_s*)p_shm)->CitiesSemName , DBShmemPriceData->SemName().c_str() , NAME_LEN - 1 );
    //((ControlDBPrice_s*)p_shm)->DBUpdated = true;
    ((ControlDBPrice_s*)p_shm)->DBUpdateRequired = false;
   }

  /* The shared queue and its semaphore must be loaded anyway independently if the shared memory exists or not due to no city exist in database. */
  strncpy( ((ControlDBPrice_s*)p_shm)->ReportQueueName , DBShmemPriceData->ReportQueueName().c_str(), NAME_LEN - 1);
  strncpy( ((ControlDBPrice_s*)p_shm)->ReportSemName   , DBShmemPriceData->ReprotQSemName().c_str(), NAME_LEN - 1);

  /* Freeing temporary-loaded-list of cityes after it was received and loaded to shared memory. If it was received as empty it will be checked inside the function "FreeList()" before clearing. */
  FreeList(&ListOfCities);  /* No need to compare the list to nullptr because it is compared in the procedure itself. Even more it should be run anyway without any condition to prevent emergency memory leakage. */
 }

void DBShmemPriceData_c::CheckMessageExistance(sqlite3 **conn)
 {
  // bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  unsigned int prio;
  ClientQueueMsg_s ClientQueueMsg;  /* Customer Acknowledge Information. */
  struct timespec ts;

  if (clock_gettime(CLOCK_REALTIME, &ts) != -1) 
   {
    ++ts.tv_sec;
   }
  
  ssize_t bytes_read = mq_timedreceive(p_sq, (char*)&ClientQueueMsg, sizeof(ClientQueueMsg_s), &prio, &ts);
  //std::cout << "Num queue received bytes " << bytes_read << "\n\r";
  if(bytes_read >= 0)
   {
    std::cout << "City: " << (StdOutNoPiping ? CITYNAME_COLOR : "") << ClientQueueMsg.City_Name << (StdOutNoPiping ? TermColorsReset : "") << "    Price: " << (StdOutNoPiping ? PRICE_COLOR : "") << ClientQueueMsg.AccumulatedPrice << (StdOutNoPiping ? PRICEUNITS_COLOR : "") << " ag"<< (StdOutNoPiping ? TermColorsReset : "") << "\n\r";
    AddOrUpdateParkingSession(conn, ClientQueueMsg);
   }
 }


void DBShmemPriceData_c::AddOrUpdateParkingSession(sqlite3 **conn, ClientQueueMsg_s &ClientQueueMsg)
 {
  bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */

  int result = 0;
  char timedurbuf[100], vehidbuf[50];

  CreateVehIDFormated(vehidbuf, sizeof(vehidbuf), ClientQueueMsg.Vechicle_ID, StdOutNoPiping);
  CreateLoadDatabase(conn); // Yes, the given pointer to database must be given as pointer to pointer to database because it's address is updated in this function.
  result = UpdateParkSessionInDataBase(conn, ClientQueueMsg);
  if(result == 0)  /* The reqauired parking session allready exists in database. */
   {
    std::ios old_state(nullptr);
    old_state.copyfmt(std::cout); 
    char old_fill = std::cout.fill();

    std::cout << (StdOutNoPiping ? ResultColors[E_CORRECT] : "") << "The parking session of "<< ClientQueueMsg.Customer_Name << " " << vehidbuf << (StdOutNoPiping ? ResultColors[E_CORRECT] : "") << " was already existing. Were updated only time and price." << (StdOutNoPiping ? TermColorsReset : "") << "\n\r";
    std::cout << (StdOutNoPiping ? ResultColors[E_CORRECT] : "") << "The parking price was updated to: " << (StdOutNoPiping ? PRICE_COLOR : "") << ClientQueueMsg.AccumulatedPrice / 100 << "." << std::setfill('0') << std::setw(2) << ClientQueueMsg.AccumulatedPrice % 100 << " " << (StdOutNoPiping ? PRICEUNITS_COLOR : "") << "₪" << (StdOutNoPiping ? ResultColors[E_CORRECT] : "") << "." << (StdOutNoPiping ? TermColorsReset : "") << "\n\r";

    // if(StdOutNoPiping)fprintf(stdout, "%s", ResultColors[E_CORRECT]);
    // printf("The parking session of %s %s was already existing. Were updated only time and price.\n\r", ClientQueueMsg.Customer_Name, vehidbuf);
    // printf("The parking price was updated to:%d.%02d ₪.\n\r", ClientQueueMsg.AccumulatedPrice / 100, ClientQueueMsg.AccumulatedPrice % 100);
    // if(StdOutNoPiping)fprintf(stdout, "%s", TermColorsReset);    

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

      // if(StdOutNoPiping)fprintf(stdout, "%s", ResultColors[E_CORRECT]);
      // printf("The parking session of %s %s starting at %s was added successfully. \n\r", ClientQueueMsg.Customer_Name, vehidbuf, timedurbuf);
      // if(StdOutNoPiping)fprintf(stdout, "%s", TermColorsReset);    

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

// DBShMemCont_c::DBShMemCont_c()
//  :ShSemMem_c(sizeof(ControlDBPrice_s))
//  {
//  }

// DBShMemCont_c::DBShMemCont_c(key_t sh_mem_key, const char sem_name[])
//  :ShSemMem_c(sh_mem_key, sem_name, sizeof(ControlDBPrice_s))
//  {
//  }

// DBShMemCont_c::~DBShMemCont_c()
//  {
//  }

// void DBShMemCont_c::SetNewShmKey(key_t KeyToSet)
//  {
//   sem_wait(p_shs);
//   ((ControlDBPrice_s*)p_shm)->CitiesNewShmKey = KeyToSet;
//   sem_post(p_shs);
//  }

// key_t DBShMemCont_c::GetNewShmKey()
//  {
//   key_t result;
//   sem_wait(p_shs);
//   result = ((ControlDBPrice_s*)p_shm)->CitiesNewShmKey;
//   sem_post(p_shs);
//   return result;
//  }


// void DBShMemCont_c::SetPriceDBSeize(uint16_t SizeToSet)
//  {
//   sem_wait(p_shs);
//   ((ControlDBPrice_s*)p_shm)->NumPriceDBCities = SizeToSet;
//   sem_post(p_shs);
//  }

// uint16_t DBShMemCont_c::GetPriceDBSeize()
//  {
//   uint16_t result;
//   sem_wait(p_shs);
//   result = ((ControlDBPrice_s*)p_shm)->NumPriceDBCities;
//   sem_post(p_shs);
//   return result;
//  }

// void DBShMemCont_c::SetUpdReqState(bool StateToSet)
//  {
//   sem_wait(p_shs);
//   ((ControlDBPrice_s*)p_shm)->DBUpdateRequired = StateToSet;
//   sem_post(p_shs);
//  }

// bool DBShMemCont_c::GetUpdReqState()
//  {
//   bool result;
//   sem_wait(p_shs);
//   result = ((ControlDBPrice_s*)p_shm)->DBUpdateRequired;
//   sem_post(p_shs);
//   return result;
//  }

// void DBShMemCont_c::SetDBUpdated(bool StateToSet)
//  {
//   sem_wait(p_shs);
//   ((ControlDBPrice_s*)p_shm)->DBUpdated = StateToSet;
//   sem_post(p_shs);
//  }

// bool DBShMemCont_c::GetDBUpdated()
//  {
//   bool result;
//   sem_wait(p_shs);
//   result = ((ControlDBPrice_s*)p_shm)->DBUpdated;
//   sem_post(p_shs);
//   return result;
//  }




DBShmemPriceData_c::DBShmemPriceData_c(int NCities)
 :ShSemMem_c(NCities*sizeof(PriceTab_s))
 {
  LoadShq(QUEUE_RECEIVE_E);
  LoadShqs();
 }
  
DBShmemPriceData_c::DBShmemPriceData_c(key_t sh_mem_key, const char sem_name[], std::string sq_name, std::string qsem_name, uint16_t NCities)
 :ShSemMem_c(sh_mem_key, sem_name, NCities * sizeof(PriceTab_s))
 {
  this->sq_name = sq_name;
  this->qsem_name = qsem_name;
  LoadShq(QUEUE_SEND_E);
  LoadShqs();
 }
  
DBShmemPriceData_c::~DBShmemPriceData_c()
 {
  RemoveShq();
  RemoveShqs();
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


std::string &DBShmemPriceData_c::ReportQueueName()
 {
  return sq_name;
 }

std::string &DBShmemPriceData_c::ReprotQSemName()
 {
  return qsem_name;
 }








void DBShmemPriceData_c::LoadShq(QueueDirection_e SendReceive)
 {
  bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */

  struct mq_attr attr;

  /* Define queue attributes */
  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;                         // Maximum messages in queue
  attr.mq_msgsize = sizeof(ClientQueueMsg_s);  // Maximum size of any message
  attr.mq_curmsgs = 0;

  std::string const basic_name = QUEUE_NAME;
  if(created)  /* "Master" Side */
   {
    GenShQueName(basic_name, sq_name);
   }
  else         /* "Slave" Side  */
   {
   }
  switch(SendReceive)
   {
    case QUEUE_SEND_E:
      p_sq = mq_open(sq_name.c_str(), O_CREAT | O_WRONLY, 0644, &attr);
     break;
    case QUEUE_RECEIVE_E:
      p_sq = mq_open(sq_name.c_str(), O_CREAT | O_RDONLY, 0644, &attr);
     break;
   }
  if (p_sq == (mqd_t)(-1)) 
   {
    perr() << (StdErrNoPiping ? TermRed : "") << "mq_open failed" << (StdErrNoPiping ? TermColorsReset : "");
    //exit(1);
   }
  else
   {
    std::cout << (StdOutNoPiping ? TermBrightBlue : "") << "The queue was opened successfully: "<< (StdOutNoPiping ? TermBrightCyan : "") << p_sq << " " << sq_name << (StdOutNoPiping ? TermColorsReset : "") << "\n\r";
   }

 }

void DBShmemPriceData_c::LoadShqs()
 {
  bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */

  if(created)  /* "Master" Side */
   {
    GenShSemKeyID(sh_qsem_key, qsem_name,  p_shqs);
    std::cout << (StdOutNoPiping ? TermBrightBlue : "") << "The semaphore was generated and created successfully: "<< (StdOutNoPiping ? TermBrightCyan : "") << p_shqs << " " << qsem_name << (StdOutNoPiping ? TermColorsReset : "") << "\n\r";
   }
  else         /* "Slave" Side  */
   {
    p_shqs = sem_open(qsem_name.c_str(), 0, 0600);
    if(p_shqs == SEM_FAILED)
     {
      perr() << (StdErrNoPiping ? TermRed : "") << "The semaphore wasn't created" << (StdErrNoPiping ? TermColorsReset : "");
      //ShSnc = true; /* Shared Semaphore not created */
      return;
     }
    std::cout << (StdOutNoPiping ? TermBrightBlue : "") << "The semaphore was opened successfully: "<< (StdOutNoPiping ? TermBrightCyan : "") << p_shqs << " " << qsem_name << (StdOutNoPiping ? TermColorsReset : "") << "\n\r";
   }
  sem_post(p_shqs);
 }

void DBShmemPriceData_c::RemoveShq()
 {
  if(created)  /* "Master" Side */
   {
   }
  else         /* "Slave" Side  */
   {
   }
  mq_close(p_sq);
  mq_unlink(sq_name.c_str()); /* Removes queue from system completely */
 }

void DBShmemPriceData_c::RemoveShqs()
 {
  if(created)  /* "Master" Side */
   {
    sem_unlink(qsem_name.c_str());
   }
  else         /* "Slave" Side  */
   {
   }
 }


void DBShmemPriceData_c::SndClientParkingInfo(Customer_s *CustomerInfo, CustAcknowledge_s *CustAckInfo)
 {
  ClientQueueMsg_s ClientMsg;
  size_t Len;

  ClientMsg.Cords               =  CustomerInfo->Cords;
  strcpy(ClientMsg.Customer_Name,  CustomerInfo->Customer_Name);
  ClientMsg.Vechicle_ID         =  CustomerInfo->Vechicle_ID;

  ClientMsg.ParkingStartTime    =  CustAckInfo->ParkingStartTime;
  ClientMsg.ParkingEndTime      =  CustAckInfo->ParkingEndTime;
  ClientMsg.ParkingDurationTime =  CustAckInfo->ParkingDurationTime;
  ClientMsg.OSM_ID              =  CustAckInfo->OSM_ID;
  ClientMsg.City_ID             =  CustAckInfo->City_ID;
  strcpy(ClientMsg.City_Name    ,  CustAckInfo->City_Name);
  ClientMsg.AccumulatedPrice    =  CustAckInfo->AccumulatedPrice;
  ClientMsg.PricePerHour        =  CustAckInfo->PricePerHour;

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
