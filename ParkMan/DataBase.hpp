#pragma once

/*======================================================================================================================*/

#include "cstdint"
#include "main.hpp"

#include "Processes.hpp"

#include "CommonData.h"
#include <sqlite3.h>


#include <sys/types.h>

/*======================================================================================================================*/

#define DB_PROC_NAME     (char *)"DataBase"     /* Database process name*/

/*======================================================================================================================*/


struct CustomerTab_s
 {
  GPS_Cords_s Cords;
  uint32_t Customer_ID;
  uint16_t Fee;                /*  0.01₪  */
 };

/*======================================================================================================================*/

/*
 * *************************************************************************************************************
 **          Shared memory, queue and semaphores class for controlling the cieies' price list stored there.
 * *************************************************************************************************************
 */

class DBShmemPriceData_c:public ShSemMemQue_c
 {
  protected:
    uint16_t OldNumCities = 0;
    Process_c *DbsCl = nullptr;
  public:
    DBShmemPriceData_c(int NCities, Process_c *DbsCl_ToSet);                                                                                           /* The initilizing constructor of the DBShmemPriceData_c Object From the Master Side.                                   */
    DBShmemPriceData_c(key_t sh_mem_key, const char sem_name[], std::string sq_name, std::string qsem_name, uint16_t NCities, Process_c *DbsCl_ToSet); /* The initilizing constructor of the DBShmemPriceData_c Object From the Slave Side.                                    */
    ~DBShmemPriceData_c();                                                                                                                             /* The deinitilizing distructor of the DBShmemPriceData_c Object.                                                       */
    void ReallocateShmem(uint16_t NewNumCities, key_t new_sh_mem_key = 0);                                                                             /* Reallocates shared memory of the database cities' parking prices in case of the memory was changed.                  */
    void LoadCitiesList(PriceTab_s ListOfCities[], int ListSize);                                                                                      /* Loads the new list of cities to the shared memory. If the memory was changed it is reallocated.                      */
    void GetCity(uint16_t CityNo, PriceTab_s *CityPriceInfo);                                                                                          /* Gives the city's info storred in the shared memory.                                                                  */
    DBShmemPriceData_c& operator = (const DBShmemPriceData_c &other) = delete;
    DBShmemPriceData_c(const DBShmemPriceData_c &other) = delete;
    void SndClientParkingInfo(Customer_s *CustomerInfo, CustAcknowledge_s *CustAckInfo);                                                               /* Sends the updated parking session client's information by the queue.                                                 */
    void CheckMessageExistance(sqlite3 **conn);                                                                                                        /* Checks exeistance of the client-parking-update-message exists in the queue.                                          */
    void AddOrUpdateParkingSession(sqlite3 **conn, ClientQueueMsg_s &ClientQueueMsg);                                                                  /* Updates the parking session of a specific client: The EndTime, ParkingDuration and the Price in the DataBase File.   */
 };

/*======================================================================================================================*/


/**
 * @brief Main Process' operating procedure.
 * 
 * @code
 * void DataBaseProc(key_t sh_mem_key, const char sem_name[], std::string sq_name, std::string qsem_name, ProcTypeID_e ProcType);
 * @code
 * 
 * @param sh_mem_key  Shared memory key for process usage.
 * 
 * @param msem_name   Shared semaphore name for using with shared memory to which is referred the parameter "sh_mem_key".
 * 
 * @param sh_que_name Shared queue for sending log messages.
 * 
 * @param qsem_name   Shared semaphore name for usage with shared queue to which is referred the parameter "sh_que_name".
 * 
 * @param ProcType    The enumeration containing the enumerated types of the additional processes.
 */
void DataBaseProc(key_t sh_mem_key, const char msem_name[], std::string sq_name, std::string qsem_name, ProcTypeID_e ProcType);



/*======================================================================================================================*/
 