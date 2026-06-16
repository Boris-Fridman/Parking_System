#include "I2C.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "CommonData.h"
#include "Processes.h"

#include "Network.h"


#define MAX_LATITUDE               90             /*  ±90˚   */
#define MAX_LONGIGUDE              180            /*  ±180˚  */

#define RESOLUTIONS                10000000000  




void InitI2C();
void DoI2C();
void CloseI2C();




void I2CProc(key_t sh_mem_key, char sem_name[])
 {
  Customer_s CustomerData;
  NetQueue_s NetQueue = {0};
  SlaveShMem_s SlaveShMem;

  ActivateSlaveShMem(&SlaveShMem, sh_mem_key, sem_name, sizeof(TskContShmData_s));
  
  strcpy(CustomerData.Name, "Boris Fridman");
  CustomerData.Vechicle_ID = 13248551;

  printf("Staritng I2C Task.\n\r");
  InitNetQueue(&NetQueue, QUEUE_SEND_E);
  
  while((getppid() != 1) && (get_flag((TskContShmData_s*)SlaveShMem.p_shm, PROC_I2C_E) != true))
   {
    /* Generating random GPS coordingates. Later will be moved to the STM32 Program with zone-dependent. */
    CustomerData.Cords.Latitude  = (RandGenLongLong() % (MAX_LATITUDE  * 2 * RESOLUTIONS)) * 1.0 / RESOLUTIONS - MAX_LATITUDE ;
    CustomerData.Cords.Longitude = (RandGenLongLong() % (MAX_LONGIGUDE * 2 * RESOLUTIONS)) * 1.0 / RESOLUTIONS - MAX_LONGIGUDE;


    printf("Customer name: %s Vehicle: %d\n\r", CustomerData.Name, CustomerData.Vechicle_ID);
    printf("The generated cordinates are: ");
    PrintGPSCords(CustomerData.Cords);
    printf("\n\r");

    /* Senging coordingates to the network process. */
    SendMessageToNetwork(&NetQueue, &CustomerData, sizeof(CustomerData));
    /* Making delay between sendings. */
    sleep(10);
   }
  CloseNetQueue(&NetQueue);
  DeactivateSlaveShMem(&SlaveShMem);

  printf("Finishing I2C Task.\n\r");
 }


 void InitI2C()
  {

  }

void DoI2C()
 {

 }

void CloseI2C()
 {

 }


