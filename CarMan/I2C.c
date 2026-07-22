#include "I2C.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "CommonData.h"
#include "Processes.h"

#include "Network.h"

#ifdef BEAGLE_BONE
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#else
#include "Israel_Shape.h"
#endif


#define MAX_LATITUDE              (  33.0 + 17.0/60.0  )  /*  Israel Limit in degrees minutes seconds (˚ ' ")  */         /*  ±90˚   */    // (  90  )
#define MIN_LATITUDE              (  29.0 + 29.0/60.0  )  /*  Israel Limit in degrees minutes seconds (˚ ' ")  */         /*  ±90˚   */    // ( -90  )

#define MAX_LONGIGUDE             (  35.0 + 54.0/60.0  )  /*  Israel Limit in degrees minutes seconds (˚ ' ")  */         /*  ±180˚  */    // (  180 )
#define MIN_LONGIGUDE             (  34.0 + 17.0/60.0  )  /*  Israel Limit in degrees minutes seconds (˚ ' ")  */         /*  ±180˚  */    // ( -180 )

#define RESOLUTIONS                10000000000  





void InitI2C();
void DoI2C();
void CloseI2C();

#ifdef BEAGLE_BONE
#else
#endif


#ifdef BEAGLE_BONE
void read_i2c(int file, unsigned char buffer[], size_t size) 
 {
  ssize_t result;

  result = read(file, buffer, size);
  printf("Where read %d bytes.\n\r",result);
  if (result != (ssize_t)size) 
   {
    perror("Failed to read from the i2c bus");
   }
 }
#else
#endif



void I2CProc(key_t sh_mem_key, char sem_name[])
 {
  Customer_s CustomerData;
  NetQueue_s NetQueue = {0};
  SlaveShMem_s SlaveShMem;
#ifdef BEAGLE_BONE
  /* Specify the I2C slave address  */
  int addr = I2C_ADDR; /* Device address */
  int file;
  //int ParkPlace;
  ParkingData_s Parking;
#else
#endif

  printf("Starting I2C Task.\n\r");

#ifdef BEAGLE_BONE
  /* Open the I2C bus device file (e.g., "/dev/i2c-2")  */
  if ((file = open("/dev/i2c-2", O_RDWR)) < 0) {
      perror("Failed to open the i2c bus");
      exit(1);
  }
  if (ioctl(file, I2C_SLAVE, addr) < 0) {
      perror("Failed to acquire bus access and/or talk to slave");
      exit(1);
  }
#else
#endif

  
  ActivateSlaveShMem(&SlaveShMem, sh_mem_key, sem_name, sizeof(TskContShmData_s));
  
  strcpy(CustomerData.Customer_Name, "Boris Fridman");
  CustomerData.Vechicle_ID = 13248551;

  InitNetQueue(&NetQueue.mq, QUEUE_SEND_E);
  
  while((getppid() != 1) && (get_flag((TskContShmData_s*)SlaveShMem.p_shm, PROC_I2C_E) != true))
   {
#ifdef BEAGLE_BONE
    // /* Generating random GPS coordingates. */
    // CustomerData.Cords.Latitude  = ( fmod( RandGenLongLong() , ((MAX_LATITUDE  - MIN_LATITUDE ) * RESOLUTIONS) ) ) * 1.0 / RESOLUTIONS + MIN_LATITUDE ;
    // CustomerData.Cords.Longitude = ( fmod( RandGenLongLong() , ((MAX_LONGIGUDE - MIN_LONGIGUDE) * RESOLUTIONS) ) ) * 1.0 / RESOLUTIONS + MIN_LONGIGUDE;

    /* Reading random generated GPS coordingates from STM32 Board. */
    read_i2c(file, (uint8_t *)&Parking, sizeof(Parking));
    CustomerData.Cords.Latitude  = Parking.ParkingCords.Latitude;
    CustomerData.Cords.Longitude = Parking.ParkingCords.Longitude;

#else
    /* Generating random GPS coordingates. */
    // PointState_e Result;
    // do
    //  {
    //    /* code */
    //   CustomerData.Cords.Latitude  = ( fmod( RandGenLongLong() , ((MAX_LATITUDE  - MIN_LATITUDE ) * RESOLUTIONS) ) ) * 1.0 / RESOLUTIONS + MIN_LATITUDE ;
    //   CustomerData.Cords.Longitude = ( fmod( RandGenLongLong() , ((MAX_LONGIGUDE - MIN_LONGIGUDE) * RESOLUTIONS) ) ) * 1.0 / RESOLUTIONS + MIN_LONGIGUDE;
    //   Result = PointInPoly(IsraelShape, NUM_IL_SHAPE_PNT, CustomerData.Cords);
    //  } 
    // while (Result != INSIDE_E);
    //32.07841°, 34.77555°


    CustomerData.Cords.Longitude  = 34.9355;  //34.77555;
    CustomerData.Cords.Latitude   = 29.5434;  //32.07841;
#endif

    printf("Customer name: %s Vehicle: %d\n\r", CustomerData.Customer_Name, CustomerData.Vechicle_ID);
    printf("The generated cordinates are: ");
    PrintGPSCords(CustomerData.Cords);
    printf("\n\r");

    /* Senging coordingates to the network process. */
    SendMessageToNetwork(&NetQueue, &CustomerData, sizeof(CustomerData));
    /* Making delay between sendings. */
    sleep(10);
   }
  CloseNetQueue(&NetQueue.mq);
  DeactivateSlaveShMem(&SlaveShMem);

#ifdef BEAGLE_BONE
  close(file);
#else
#endif

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


