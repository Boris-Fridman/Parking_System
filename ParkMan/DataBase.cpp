#include "DataBase.hpp"
#include "main.hpp"
#include "Errors.hpp"
#include "Processes.hpp"
#include "PriceDataBase.h"

#include <semaphore.h>
#include <unistd.h>
#include <iostream>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sqlite3.h>

#define DB_PROC_NAME     (char *)"DataBase"     /* Database process name*/



/*======================================================================================================================*/

class DataBase_c: public Process_c
 {
    DBShmemPriceData_c *DBShmemPriceData = NULL;
    sqlite3 *conn = NULL;
  public:
    DataBase_c(char ProcName[], key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType);
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
void DataBaseProc(key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType)  //  void DataBaseProc(key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType)
 {
  DataBase_c DB_Process(DB_PROC_NAME, sh_mem_key, sem_name, ProcType);
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
  
  if(DataBaseMusgBeReloaded())
   {
    LoadDataBase();
   }
 };

void DataBase_c::OnFinishProcess()
 {
  delete DBShmemPriceData;
 }


DataBase_c::DataBase_c(char ProcName[], key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType)
 :Process_c(ProcName, sh_mem_key, sem_name, ProcType)
 {
  
 }

DataBase_c::~DataBase_c()
 {

 }






void DataBase_c::LoadDataBase()
 {
  std::string DBFileName;
  int result;
  PriceTab_s *ListOfCities = NULL;
  int NumCities;
  
  DBFileName = GetDBFileName();
  SetDBPathName(DBFileName.c_str()); /* Loading name of the file contains the database of prices with the parking systems. */
  result = GetCitiesList(&conn, &ListOfCities, &NumCities);
  if((result == 0)&&(NumCities > 0))
   {
    DBShmemPriceData->LoadCitiesList(ListOfCities, NumCities);
    ((ControlDBPrice_s*)p_shm)->NumPriceDBCities = NumCities;
    ((ControlDBPrice_s*)p_shm)->CitiesNewShmKey = DBShmemPriceData->ShMemKey();
    const char *SmNm = DBShmemPriceData->SemName().c_str();
    strcpy( ((ControlDBPrice_s*)p_shm)->CitiesSemName , SmNm );
    //((ControlDBPrice_s*)p_shm)->DBUpdated = true;
    ((ControlDBPrice_s*)p_shm)->DBUpdateRequired = false;
   }
  FreeList(&ListOfCities);  /* No need to compare the list to NULL because it is compared in the procedure itself. Even more it should be run anyway without any condition to prevent emergency memory leakage. */
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
 }
  
DBShmemPriceData_c::DBShmemPriceData_c(key_t sh_mem_key, const char sem_name[], uint16_t NCities)
 :ShSemMem_c(sh_mem_key, sem_name, NCities * sizeof(PriceTab_s))
 {
 }
  
DBShmemPriceData_c::~DBShmemPriceData_c()
 {
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

/*======================================================================================================================*/
