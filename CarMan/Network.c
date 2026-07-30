#include "Network.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#include <unistd.h>
#include <errno.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "CommonData.h"
#include "Processes.h"

#include "Configuration.h"


#define QUEUE_NAME     "/network_queue"
#define MAX_SIZE       1024




bool StartNetwork(NetworkParams_s *NetPars, bool AllwaysTermEnabled);
void DoNetwork(SlaveShMem_s *SlaveShMem, NetworkParams_s *NetPars, NetQueue_s *NetQ);
bool SendToNetwork(NetworkParams_s *NetPars, void *Data, size_t Len);
void CloseNetwork(NetworkParams_s *NetPars);



void NetworkProc(key_t sh_mem_key, char sem_name[])
 {
  NetworkParams_s NetworkParams = {0};
  NetQueue_s NetQueue = {0};
  SlaveShMem_s SlaveShMem;

  ActivateSlaveShMem(&SlaveShMem, sh_mem_key, sem_name, sizeof(TskContShmData_s));

  InitNetQueue(&NetQueue.mq, true);

  DoNetwork(&SlaveShMem, &NetworkParams, &NetQueue);

  CloseNetQueue(&NetQueue.mq);
  DeactivateSlaveShMem(&SlaveShMem);
 }




bool StartNetwork(NetworkParams_s *NetPars, bool AllwaysTermEnabled)
 {
  bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  static int last_errno = 0;
  uint16_t DestinPort;
  bool UseDHCP = false;
  char const *DHCPName;
  char const *DestIP;
  struct hostent *host = NULL;
  //struct addrinfo hints, *res, *p;
  

  /* Create the socket */
  NetPars->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (NetPars->sock_fd < 0) 
   {
    if(AllwaysTermEnabled)
     perror("Socket creation error");
    return false;
   }

  DestinPort = GetDestinPort();

  UseDHCP = GetUseDHCPState();
  //DestIP = GetDestinAddr();
  // DHCPName = GetDestinDHCPName();
  // host = gethostbyname(DHCPName);

  // memset(&hints, 0, sizeof(hints));
  // hints.ai_family = AF_INET; // hints.ai_family = AF_UNSPEC;    // AF_UNSPEC allows both IPv4 and IPv6  
  // hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

  // getaddrinfo(DHCPName, NULL, &hints, &res);
  // struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
  // void *addr;
  // char ip_str[INET6_ADDRSTRLEN];
  // addr = &(ipv4->sin_addr);
  // inet_ntop(p->ai_family, addr, ip_str, sizeof(ip_str));
  // printf("Found IP: %s\n", ip_str);
  // char IP_Addr[20];
  // inet_ntop(AF_INET, host->h_addr_list, IP_Addr, host->h_length);



  /* Configure the server address structure */
  memset(&NetPars->server_addr, 0, sizeof(NetPars->server_addr));
  NetPars->server_addr.sin_family = AF_INET;
  NetPars->server_addr.sin_port = htons(DestinPort);  //  NetPars->server_addr.sin_port = htons(DESTIN_PORT);


  if(UseDHCP)
   {
    DHCPName = GetDestinDHCPName();
    host = gethostbyname(DHCPName);
    if(host == NULL)
     {
      fprintf(stderr, "%s", (StdErrNoPiping ? ResultColors[E_FAIL] : ""));
      fprintf(stderr, "Couldn't detect dynamic address from DHCP name %s.\n\r", DHCPName);
      fprintf(stderr, "%s", (StdErrNoPiping ? TermColorsReset : ""));
     }
   }
  
  if((UseDHCP) && (host != NULL))
   {
    fprintf(stdout, "Loading DHCP address...\n\r");
    memcpy(&NetPars->server_addr.sin_addr, host->h_addr_list[0], MIN(host->h_length, (int)sizeof(NetPars->server_addr.sin_addr)));
   }
  else
   {
    DestIP = GetDestinAddr();
    fprintf(stdout, "Loading Static IP address...\n\r");
    /* Convert IPv4 address from text to binary format */
    if (inet_pton(AF_INET, DestIP, &NetPars->server_addr.sin_addr) <= 0)   //  if (inet_pton(AF_INET, DESTIN_IP, &NetPars->server_addr.sin_addr) <= 0) 
     {
      if((AllwaysTermEnabled) || (last_errno != errno))
       {
        fprintf(stderr, "%s", (StdErrNoPiping ? ResultColors[E_FAIL] : ""));
        perror("Invalid address or Address not supported");
        fprintf(stderr, "%s", (StdErrNoPiping ? TermColorsReset : ""));
        last_errno = errno;
       }
      close(NetPars->sock_fd);
      return false;
     }
   }

  

   //inet_ntop(AF_INET, host->h_addr_list, IP_Addr, host->h_length);


  uint8_t ad[4];
  memcpy(ad, &NetPars->server_addr.sin_addr, 4);
  
  printf("Trying to connect to the address %d.%d.%d.%d with port %d ...\n\r", ad[0], ad[1], ad[2], ad[3] ,DestinPort);
  //printf("Trying to connect to the address %s with port %d ...\n\r", DestIP ,DestinPort);

  /* Connect to the server */
  if (connect(NetPars->sock_fd, (struct sockaddr *)&NetPars->server_addr, sizeof(NetPars->server_addr)) < 0) 
   {
    if((AllwaysTermEnabled) || (last_errno != errno))
     {
      fprintf(stderr, "%s", (StdErrNoPiping ? ResultColors[E_FAIL] : ""));
      perror("Connection Failed");
      fprintf(stderr, "%s", (StdErrNoPiping ? ResultColors[E_WARNING] : ""));
      fprintf(stderr, "Recheck server\n\r");
      fprintf(stderr, "%s", (StdErrNoPiping ? TermColorsReset : ""));
      last_errno = errno;
     }
    close(NetPars->sock_fd);
    return false;
   }
  printf("%sConnected successfully to the server.%s\n\r", (StdOutNoPiping ? ResultColors[E_CORRECT] : ""), (StdOutNoPiping ? TermColorsReset : ""));  /* Must be printed anyway */
  return true;
 }

bool SendToNetwork(NetworkParams_s *NetPars, void *Data, size_t Len)    /*  Send ▬▬▬▶ Network   ⸺▶    Wait for response   ⸺▶   Network ▬▬▬▶ Receive */
 {
  bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */

  char buffer[BUFFER_SIZE] = {0}, timedurbuf[100];
  CustAcknowledge_s CustAckInfo;
  bool DecodeResult;
  ssize_t nsndbt;
  ssize_t bytes_read;
  bool Result;
  uint8_t *DataForSending;
  ssize_t SentDataSize;
  
  SentDataSize = EncodeNetData(Data, Len, &DataForSending);
  printf("%sData was encoded.%s\n\r", TermBlue, TermColorsReset);
  /* Send data to the server */
  nsndbt = send(NetPars->sock_fd, DataForSending, SentDataSize, MSG_NOSIGNAL);
  printf("%sData was sent...%s\n\r", TermBlue, TermColorsReset);
  Result = (nsndbt >= 0);
  if (!Result)
   {
    fprintf(stderr, "%s", (StdErrNoPiping ? ResultColors[E_FAIL] : ""));
    perror("Send failed");
    printf("errno %d\n\r", errno);
    //EPIPE;
    fprintf(stderr, "%s", (StdErrNoPiping ? TermColorsReset : ""));
   }
  else
   {
    fprintf(stdout, "%s", (StdOutNoPiping ? ResultColors[E_SUCCESS] : ""));   
#if !defined(__arm__)
    printf("Were sent %ld bytes to the network.\n\r", SentDataSize);
#else
    printf("Were sent %d bytes to the network.\n\r", SentDataSize);
#endif
    fprintf(stdout, "%s", (StdOutNoPiping ? TermColorsReset : ""));
  
    FreeData(&DataForSending);
  
    /* Receive data back from the server */
    bytes_read = recv(NetPars->sock_fd, buffer, BUFFER_SIZE - 1, 0);
    Result = (bytes_read >= 0);
    if (bytes_read < 0) 
     {
      fprintf(stderr, "%s", (StdErrNoPiping ? ResultColors[E_FAIL] : ""));
      perror("Receive failed");
      fprintf(stderr, "%s", (StdErrNoPiping ? TermColorsReset : ""));
     } 
    else 
     if (bytes_read == 0) 
      {
       printf("%sServer closed the connection.%s\n\r", (StdErrNoPiping ? ResultColors[E_FAIL] : ""), (StdErrNoPiping ? TermColorsReset : ""));
       Result = false;
      } 
     else 
      {
       buffer[bytes_read] = '\0'; // Null-terminate the received string
  
#if !defined(__arm__)
       printf("Server responded data contains %ld bytes.\n\r", bytes_read);
#else
       printf("Server responded data contains %d bytes.\n\r", bytes_read);
#endif
       DecodeResult = DecodeNetData((uint8_t*)buffer, bytes_read, (uint8_t *)&CustAckInfo);
       if(DecodeResult)
        {
         printf("The vehicle is parked in: %s   (ID: %d)\n\r", CustAckInfo.City_Name, CustAckInfo.City_ID);
         //  printf("Vehicle ID: %s%s%d%s\n\r", TermBGYello, TermBlack, CustAckInfo.Vechicle_ID, TermColorsReset);
         //VehicleIDToString(buffer, sizeof(buffer), CustAckInfo.Vechicle_ID);
         //printf("Vehicle ID: %s%s%s%s\n\r", TermBGYello, TermBlack, buffer, TermColorsReset);
         CreateVehIDFormated(buffer, sizeof(buffer), CustAckInfo.Vechicle_ID, StdOutNoPiping);
         printf("Vehicle ID: %s%s\n\r", buffer, (StdErrNoPiping ? TermColorsReset : ""));
  
         ConvertTime(&CustAckInfo.ParkingStartTime, timedurbuf, sizeof(timedurbuf), E_CAL_FORMAT);
         printf("Parking started at: %s\n\r", timedurbuf);
         ConvertTime(&CustAckInfo.ParkingDurationTime, timedurbuf, sizeof(timedurbuf), E_DUR_FORMAT);
         printf("Parking duration: %s   price: %s%d.%02d%s₪%s\n\r", timedurbuf, (StdOutNoPiping ? TermBlue : ""), CustAckInfo.AccumulatedPrice / 100, CustAckInfo.AccumulatedPrice % 100, (StdOutNoPiping ? TermMagenta : ""), (StdOutNoPiping ? TermColorsReset : ""));
        }
       else
        {
         printf("%sError in response.%s\n\r", (StdErrNoPiping ? ResultColors[E_FAIL] : ""), (StdErrNoPiping ? TermColorsReset : ""));
        }
      }
   }
  return Result;
 }

void DoNetwork(SlaveShMem_s *SlaveShMem, NetworkParams_s *NetPars, NetQueue_s *NetQ)   /*  Queue  ▬▬▬▶ Network */
 {
  // bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  // bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  bool Connected = false;
  bool PrintTermOnConnection = true;
  unsigned int prio;
  char buffer[BUFFER_SIZE] = {0};
  struct timespec ts;


  get_flag((TskContShmData_s*)SlaveShMem->p_shm, PROC_NETWORK_E);


  while((getppid() != 1) && (get_flag((TskContShmData_s*)SlaveShMem->p_shm, PROC_NETWORK_E) != true))
   {

    if(!Connected)
     {
      Connected = StartNetwork(NetPars, PrintTermOnConnection);
      PrintTermOnConnection = false;
     }

    if (clock_gettime(CLOCK_REALTIME, &ts) != -1) 
     {
      ++ts.tv_sec;
     }
    /* Block until a message is received */
    ssize_t bytes_read = mq_timedreceive(NetQ->mq, buffer, MAX_SIZE, &prio, &ts);
    if (bytes_read >= 0) 
     {

#if !defined(__arm__)
      printf("Were forwarded from the queue to the network %ld bytes.\n\r", bytes_read);
#else
      printf("Were forwarded from the queue to the network %d bytes.\n\r", bytes_read);
#endif
      PrintTermOnConnection = true;
      if(Connected)
       {
        Connected = SendToNetwork(NetPars, buffer, bytes_read);
       }
      if(!Connected)
       {
        CloseNetwork(NetPars);
       }
     } 
    
   }

  CloseNetwork(NetPars);

 }

void CloseNetwork(NetworkParams_s *NetPars)
 {
  /* Close the socket connection */
  close(NetPars->sock_fd);
 }






void InitNetQueue(mqd_t *mq, QueueDirection_e SendReceive)
 {
  struct mq_attr attr;

  /* Define queue attributes */
  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;        // Maximum messages in queue
  attr.mq_msgsize = MAX_SIZE; // Maximum size of any message
  attr.mq_curmsgs = 0;
  /* Create and open the queue for writing */
  switch(SendReceive)
   {
    case QUEUE_SEND_E:
      *mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0644, &attr);
     break;
    case QUEUE_RECEIVE_E:
      *mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);
     break;
   }
   
  if (*mq == (mqd_t)(-1)) 
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
#if !defined(__arm__)
    printf("Message sent successfully. %ld bytes sent.\n\r", Len);
#else
    printf("Message sent successfully. %d bytes sent.\n\r", Len);
#endif
   }
 }

void CloseNetQueue(mqd_t *mq)
 {
  mq_close(*mq);
  mq_unlink(QUEUE_NAME); /* Removes queue from system completely */
 }
