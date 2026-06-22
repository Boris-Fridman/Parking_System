#include "Network.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "CommonData.h"
#include "Processes.h"


#define QUEUE_NAME     "/network_queue"
#define MAX_SIZE       1024




void StartNetwork(NetworkParams_s *NetPars);
void DoNetwork(SlaveShMem_s *SlaveShMem, NetworkParams_s *NetPars, NetQueue_s *NetQ);
void SendToNetwork(NetworkParams_s *NetPars, void *Data, size_t Len);
void CloseNetwork(NetworkParams_s *NetPars);



void NetworkProc(key_t sh_mem_key, char sem_name[])
 {
  NetworkParams_s NetworkParams = {0};
  NetQueue_s NetQueue = {0};
  SlaveShMem_s SlaveShMem;

  ActivateSlaveShMem(&SlaveShMem, sh_mem_key, sem_name, sizeof(TskContShmData_s));

  InitNetQueue(&NetQueue, true);

  StartNetwork(&NetworkParams);
  
  DoNetwork(&SlaveShMem, &NetworkParams, &NetQueue);

  CloseNetwork(&NetworkParams);

  CloseNetQueue(&NetQueue);
  DeactivateSlaveShMem(&SlaveShMem);
 }




void StartNetwork(NetworkParams_s *NetPars)
 {

  /* Create the socket */
  NetPars->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (NetPars->sock_fd < 0) 
   {
    perror("Socket creation error");
    exit(EXIT_FAILURE);
   }

  /* Configure the server address structure */
  memset(&NetPars->server_addr, 0, sizeof(NetPars->server_addr));
  NetPars->server_addr.sin_family = AF_INET;
  NetPars->server_addr.sin_port = htons(DESTIN_PORT);
  /* Convert IPv4 address from text to binary format */
  if (inet_pton(AF_INET, DESTIN_IP, &NetPars->server_addr.sin_addr) <= 0) 
   {
    perror("Invalid address or Address not supported");
    close(NetPars->sock_fd);
    exit(EXIT_FAILURE);
   }


  /* Connect to the server */
  if (connect(NetPars->sock_fd, (struct sockaddr *)&NetPars->server_addr, sizeof(NetPars->server_addr)) < 0) 
   {
    perror("Connection Failed");
    close(NetPars->sock_fd);
    exit(EXIT_FAILURE);
   }
  printf("Connected successfully to the server.\n\r");


 }

void SendToNetwork(NetworkParams_s *NetPars, void *Data, size_t Len)    /*  Send ▬▬▬▶ Network   ⸺▶    Wait for response   ⸺▶   Network ▬▬▬▶ Receive */
 {
  char buffer[BUFFER_SIZE] = {0}, timedurbuf[100];
  CustAcknowledge_s CustAckInfo;
  bool DecodeResult;
  ssize_t nsndbt;
  bool SendResult;
  uint8_t *DataForSending;
  ssize_t SentDataSize;
  
  SentDataSize = EncodeNetData(Data, Len, &DataForSending);
  
  /* Send data to the server */
  nsndbt = send(NetPars->sock_fd, DataForSending, SentDataSize, 0);
  SendResult = (nsndbt >= 0);
  if (SendResult)
   {
    perror("Send failed");
    //close(NetPars->sock_fd);
    //return;
   }
  



  printf("Were sent %ld bytes to the network.\n\r", SentDataSize);
  FreeData(&DataForSending);

  /* Receive data back from the server */
  ssize_t bytes_read = recv(NetPars->sock_fd, buffer, BUFFER_SIZE - 1, 0);
  if (bytes_read < 0) 
   {
    perror("Receive failed");
   } 
  else 
   if (bytes_read == 0) 
    {
     printf("%sServer closed the connection.%s\n\r", TermRed, TermColorsReset);
    } 
   else 
    {
     buffer[bytes_read] = '\0'; // Null-terminate the received string
     printf("Server responded data contains %ld bytes.\n\r", bytes_read);
     DecodeResult = DecodeNetData((uint8_t*)buffer, bytes_read, (uint8_t *)&CustAckInfo);
     if(DecodeResult)
      {
       printf("The vehicle is parked in: %s   (ID: %d)\n\r", CustAckInfo.City_Name, CustAckInfo.City_ID);
       //  printf("Vehicle ID: %s%s%d%s\n\r", TermBGYello, TermBlack, CustAckInfo.Vechicle_ID, TermColorsReset);
       //VehicleIDToString(buffer, sizeof(buffer), CustAckInfo.Vechicle_ID);
       //printf("Vehicle ID: %s%s%s%s\n\r", TermBGYello, TermBlack, buffer, TermColorsReset);
       CreateVechIDFormated(buffer, sizeof(buffer), CustAckInfo.Vechicle_ID, true);
       printf("Vehicle ID: %s%s\n\r", buffer, TermColorsReset);

       ConvertTime(&CustAckInfo.ParkingStartTime, timedurbuf, sizeof(timedurbuf), E_CAL_FORMAT);
       printf("Parking started at: %s\n\r", timedurbuf);
       ConvertTime(&CustAckInfo.ParkingDurationTime, timedurbuf, sizeof(timedurbuf), E_DUR_FORMAT);
       printf("Parking duration: %s   price: %s%d.%02d%s₪%s\n\r", timedurbuf, TermBrightBlue, CustAckInfo.AccumulatedPrice / 100, CustAckInfo.AccumulatedPrice % 100, TermMagenta, TermColorsReset);
      }
     else
      {
       printf("%sError in response.%s\n\r", TermRed, TermColorsReset);
      }

    }
 }

void DoNetwork(SlaveShMem_s *SlaveShMem, NetworkParams_s *NetPars, NetQueue_s *NetQ)   /*  Queue  ▬▬▬▶ Network */
 {
  unsigned int prio;
  char buffer[BUFFER_SIZE] = {0};
  struct timespec ts;
  get_flag((TskContShmData_s*)SlaveShMem->p_shm, PROC_NETWORK_E);


  while((getppid() != 1) && (get_flag((TskContShmData_s*)SlaveShMem->p_shm, PROC_NETWORK_E) != true))
   {
    if (clock_gettime(CLOCK_REALTIME, &ts) != -1) 
     {
      ++ts.tv_sec;
     }
    /* Block until a message is received */
    ssize_t bytes_read = mq_timedreceive(NetQ->mq, buffer, MAX_SIZE, &prio, &ts);
    if (bytes_read >= 0) 
     {
      //printf("Received message: %s\n\r", buffer);
      printf("Were forwarded from the queue to the network %ld bytes.\n\r", bytes_read);
 
      SendToNetwork(NetPars, buffer, bytes_read);
     } 
    
   }
  

 }

void CloseNetwork(NetworkParams_s *NetPars)
 {
  /* Close the socket connection */
  close(NetPars->sock_fd);
 }






void InitNetQueue(NetQueue_s *NetQ, QueueDirection_e SendReceive)
 {
  struct mq_attr attr;

  // Define queue attributes
  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;        // Maximum messages in queue
  attr.mq_msgsize = MAX_SIZE; // Maximum size of any message
  attr.mq_curmsgs = 0;
  /* Create and open the queue for writing */
  switch(SendReceive)
   {
    case QUEUE_SEND_E:
      NetQ->mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0644, &attr);
     break;
    case QUEUE_RECEIVE_E:
      NetQ->mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);
     break;
   }
   
  if (NetQ->mq == (mqd_t)-1) 
   {
    perror("mq_open failed");
    exit(1);
   }

 }

void SendMessageToNetwork(NetQueue_s *NetQ, void *Data, size_t Len)  //  Process ▬▬▬▶ Queue
 {
  if (mq_send(NetQ->mq, Data, Len, 0) == -1)
   {
    perror("mq_send failed");
   } 
  else 
   {
    printf("Message sent successfully. %ld bytes sent.\n\r", Len);
   }
 }

void CloseNetQueue(NetQueue_s *NetQ)
 {
  mq_close(NetQ->mq);
  mq_unlink(QUEUE_NAME); /* Removes queue from system completely */
 }
