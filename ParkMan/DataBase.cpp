#include "DataBase.hpp"
#include "main.hpp"
#include "Errors.hpp"
#include "Processes.hpp"

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

void DataBaseProc(key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType)
 {
  DataBase_c DB_Process(DB_PROC_NAME, sh_mem_key, sem_name, ProcType);
  DB_Process.RunProcess();
 }


void DataBase_c::OnStartProcess()
 {
  DBShmemPriceData = new DBShmemPriceData_c(0);
 }

void DataBase_c::OnRunProcess()
 {
  Process_c::OnRunProcess();
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
  // std::string DBFileName;
  // uint16_t NewNumCities = 0;
  // int result;
  
  // DBFileName = GetDBFileName();
  // result = sqlite3_open(DBFileName.c_str(), &conn);
  // if(result == SQLITE_OK)
  //  {
  //   //DBShmemPriceData->ReallocateShmem(NewNumCities);
  //   sqlite3_close(conn);
  //  }

  
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
//   ((ControlDBPrice_s*)p_shm)->NewShmKey = KeyToSet;
//   sem_post(p_shs);
//  }

// key_t DBShMemCont_c::GetNewShmKey()
//  {
//   key_t result;
//   sem_wait(p_shs);
//   result = ((ControlDBPrice_s*)p_shm)->NewShmKey;
//   sem_post(p_shs);
//   return result;
//  }


// void DBShMemCont_c::SetPriceDBSeize(uint16_t SizeToSet)
//  {
//   sem_wait(p_shs);
//   ((ControlDBPrice_s*)p_shm)->PriceDBSize = SizeToSet;
//   sem_post(p_shs);
//  }

// uint16_t DBShMemCont_c::GetPriceDBSeize()
//  {
//   uint16_t result;
//   sem_wait(p_shs);
//   result = ((ControlDBPrice_s*)p_shm)->PriceDBSize;
//   sem_post(p_shs);
//   return result;
//  }

// void DBShMemCont_c::SetUpdReqState(bool StateToSet)
//  {
//   sem_wait(p_shs);
//   ((ControlDBPrice_s*)p_shm)->UpdateRequired = StateToSet;
//   sem_post(p_shs);
//  }

// bool DBShMemCont_c::GetUpdReqState()
//  {
//   bool result;
//   sem_wait(p_shs);
//   result = ((ControlDBPrice_s*)p_shm)->UpdateRequired;
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

void DBShmemPriceData_c::ReallocateShmem(uint16_t NewNumCities)
 {
  UNUSED(NewNumCities);
  RemoveShm();
  LoadShm(NewNumCities * sizeof(PriceTab_s));
 }




/*======================================================================================================================*/
