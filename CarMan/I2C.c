#include "I2C.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "CommonData.h"

#include "Network.h"


#define MAX_LATITUDE  90  // ±90˚
#define MAX_LONGIGUDE 180 // ±180˚

#define RESOLUTIONS 10000000000  




void InitI2C();
void CloseI2C();








void I2CProc()
 {
  GPS_Cords_s Cords;
  NetQueue_s NetQueue = {0};
  

  printf("Staritng I2C Task.\n\r");
  InitNetQueue(&NetQueue, QUEUE_SEND_E);

  while(getppid() != 1)
  {
   /* Generating random GPS coordingates. Later will be moved to the STM32 Program with zone-dependent. */
   Cords.Latitude  = (rand()%(MAX_LATITUDE * 2 * RESOLUTIONS)) * 1.0 / RESOLUTIONS - MAX_LATITUDE;
   Cords.Longitude = (rand()%(MAX_LONGIGUDE * 2 * RESOLUTIONS)) * 1.0 / RESOLUTIONS - MAX_LONGIGUDE;
   //printf("The generated cordinates are: %3.8lf˚ lat %3.8lf˚ long\n\r", Cords.Latitude, Cords.Longitude);
   printf("The generated cordinates are: ");
   PrintGPSCords(Cords);
   printf("\n\r");
   /* Senging coordingates to the network process. */
   SendMessageToNetwork(&NetQueue, &Cords, sizeof(Cords));

   /* Making delay between sendings. */
   sleep(10);
  }
  CloseNetQueue(&NetQueue);

  printf("Finishing I2C Task.\n\r");
 }


 void InitI2C()
  {

  }

void CloseI2C()
 {

 }


