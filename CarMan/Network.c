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


#define QUEUE_NAME "/network_queue"
#define MAX_SIZE 1024




void StartNetwork(NetworkParams_s *NetPars);
void DoNetwork(NetworkParams_s *NetPars, NetQueue_s *NetQ);
void SendToNetwork(NetworkParams_s *NetPars, void *Data, size_t Len);
void CloseNetwork(NetworkParams_s *NetPars);



void NetworkProc()
 {
  NetworkParams_s NetworkParams = {0};
  NetQueue_s NetQueue = {0};
  InitNetQueue(&NetQueue, true);

  StartNetwork(&NetworkParams);

  
  DoNetwork(&NetworkParams, &NetQueue);
  CloseNetwork(&NetworkParams);

  CloseNetQueue(&NetQueue);
 }




void StartNetwork(NetworkParams_s *NetPars)
 {

  // 1. Create the socket
  NetPars->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (NetPars->sock_fd < 0) 
   {
    perror("Socket creation error");
    exit(EXIT_FAILURE);
   }

  // 2. Configure the server address structure
  memset(&NetPars->server_addr, 0, sizeof(NetPars->server_addr));
  NetPars->server_addr.sin_family = AF_INET;
  NetPars->server_addr.sin_port = htons(DESTIN_PORT);
  // Convert IPv4 address from text to binary format
  if (inet_pton(AF_INET, DESTIN_IP, &NetPars->server_addr.sin_addr) <= 0) 
   {
    perror("Invalid address or Address not supported");
    close(NetPars->sock_fd);
    exit(EXIT_FAILURE);
   }


  // 3. Connect to the server
  if (connect(NetPars->sock_fd, (struct sockaddr *)&NetPars->server_addr, sizeof(NetPars->server_addr)) < 0) 
   {
    perror("Connection Failed");
    close(NetPars->sock_fd);
    exit(EXIT_FAILURE);
   }
  printf("Connected successfully to the server.\n");


 }

void SendToNetwork(NetworkParams_s *NetPars, void *Data, size_t Len)
 {
  char buffer[BUFFER_SIZE] = {0};

  //char *message = "Hello from the C Client!";

  // 4. Send data to the server
  
  if (send(NetPars->sock_fd, Data, Len, 0) < 0)   //  if (send(NetPars->sock_fd, message, strlen(message), 0) < 0) 
   {
    perror("Send failed");
    close(NetPars->sock_fd);
    exit(EXIT_FAILURE);
   }
  printf("Sent fowrard coordinates: ");
  PrintGPSCords(*(GPS_Cords_s*)Data);
  printf("\n\r");
  //printf("Message sent: %s\n\r", message);
  //printf("Message sent: %s\n\r", Data);
  //printf("Message sent: %s\n\r", "GPS Coordinates");

  // 5. Receive data back from the server
  ssize_t bytes_read = recv(NetPars->sock_fd, buffer, BUFFER_SIZE - 1, 0);
  if (bytes_read < 0) 
   {
    perror("Receive failed");
   } 
  else 
   if (bytes_read == 0) 
    {
     printf("Server closed the connection.\n");
    } 
   else 
    {
     buffer[bytes_read] = '\0'; // Null-terminate the received string
     //printf("Server response: %s\n", buffer);
     printf("Received backward coordinates: ");
     PrintGPSCords(*(GPS_Cords_s*)Data);
     printf("\n\r");

    }
 }

void DoNetwork(NetworkParams_s *NetPars, NetQueue_s *NetQ)
 {
  unsigned int prio;
  char buffer[BUFFER_SIZE] = {0};
  struct timespec ts;

  while(getppid() != 1)
   {
    if (clock_gettime(CLOCK_REALTIME, &ts) != -1) 
     {
      ++ts.tv_sec;
     }
    // Block until a message is received
    ssize_t bytes_read = mq_timedreceive(NetQ->mq, buffer, MAX_SIZE, &prio, &ts);
    //ssize_t bytes_read = mq_receive(NetQ->mq, buffer, MAX_SIZE, &prio);
    if (bytes_read >= 0) 
     {
      //printf("Received message: %s\n", buffer);
      printf("Received backward coordinates: ");
      PrintGPSCords(*(GPS_Cords_s*)buffer);
      printf("\n\r");
 
      SendToNetwork(NetPars, buffer, bytes_read);
     } 
    
   }
  

 }

void CloseNetwork(NetworkParams_s *NetPars)
 {
  // 6. Close the socket connection
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
  // Create and open the queue for writing
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

void SendMessageToNetwork(NetQueue_s *NetQ, void *Data, size_t Len)
 {
  char buffer[BUFFER_SIZE] = {0};
  printf("The received coordinates are: ");
  PrintGPSCords(*(GPS_Cords_s*)Data);
  printf("\n\r");
  //snprintf(buffer, sizeof(buffer), "Hello from the Sender Process!\n\r");
  // Send message with priority 0
  if (mq_send(NetQ->mq, Data, Len, 0) == -1) //  if (mq_send(NetQ->mq, buffer, strlen(buffer) + 1, 0) == -1) 
   {
    perror("mq_send failed");
   } 
  else 
   {
    printf("Message sent successfully.\n");
   }
 }

void CloseNetQueue(NetQueue_s *NetQ)
 {
  mq_close(NetQ->mq);
  mq_unlink(QUEUE_NAME); // Removes queue from system completely
 }
