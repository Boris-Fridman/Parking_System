#pragma once

#include "cstdint"
#include "main.hpp"

#include "Processes.hpp"

#include "CommonData.h"



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
  public:
    DBShmemPriceData_c(int NCities);
    DBShmemPriceData_c(key_t sh_mem_key, const char sem_name[], uint16_t NCities);
    ~DBShmemPriceData_c();
  protected:
    void ReallocateShmem(uint16_t NewNumCities);
  public:
    void LoadCitiesList(PriceTab_s ListOfCities[], int ListSize);
 };










 void DataBaseProc(key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType);


 