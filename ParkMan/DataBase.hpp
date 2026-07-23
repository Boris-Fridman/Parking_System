#pragma once

#include "cstdint"
#include "main.hpp"

#include "Processes.hpp"

#include "CommonData.h"
#include <sqlite3.h>


#include <sys/types.h>

struct CustomerTab_s
 {
  GPS_Cords_s Cords;
  uint32_t Customer_ID;
  uint16_t Fee;                /*  0.01₪  */
 };


// class DBShMemCont_c:public ShSemMem_c
//  {

//   public:
//     DBShMemCont_c();
//     DBShMemCont_c(key_t sh_mem_key, const char sem_name[]);
//     ~DBShMemCont_c();
//     void SetNewShmKey(key_t KeyToSet);
//     key_t GetNewShmKey();
//     void SetPriceDBSeize(uint16_t SizeToSet);
//     uint16_t GetPriceDBSeize();
//     void SetUpdReqState(bool StateToSet);
//     bool GetUpdReqState();
//     void SetDBUpdated(bool StateToSet);
//     bool GetDBUpdated();
//  };

class DBShmemPriceData_c:public ShSemMem_c
 {
  protected:
    uint16_t OldNumCities = 0;

    /* Report shared queue variables */
    std::string sq_name = "";   /* Shared Queue name */
    mqd_t p_sq = 0;             /* Shared Queue variable */
    
    /* Report shared queue semaphore variables */
    key_t sh_qsem_key = 0;      /* Queue shared semaphore key        */     
    sem_t *p_shqs = nullptr;    /* Pointer to Queue shared semaphore */
    std::string qsem_name = ""; /* Queue shared semaphore name       */

    void LoadShq(QueueDirection_e SendReceive);
    void LoadShqs();
    void RemoveShq();
    void RemoveShqs();


  public:
    DBShmemPriceData_c(int NCities);
    DBShmemPriceData_c(key_t sh_mem_key, const char sem_name[], std::string sq_name, std::string qsem_name, uint16_t NCities);
    ~DBShmemPriceData_c();
    void ReallocateShmem(uint16_t NewNumCities, key_t new_sh_mem_key = 0);
    void LoadCitiesList(PriceTab_s ListOfCities[], int ListSize);
    void GetCity(uint16_t CityNo, PriceTab_s *CityPriceInfo);
    std::string ReportQueueName();
    std::string ReprotQSemName();
    DBShmemPriceData_c& operator = (const DBShmemPriceData_c &other) = delete;
    DBShmemPriceData_c(const DBShmemPriceData_c &other) = delete;
    void SndClientParkingInfo(Customer_s *CustomerInfo, CustAcknowledge_s *CustAckInfo);
    void CheckMessageExistance(sqlite3 **conn);
    void AddOrUpdateParkingSession(sqlite3 **conn, ClientQueueMsg_s &ClientQueueMsg);
 };










void DataBaseProc(key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType);




 