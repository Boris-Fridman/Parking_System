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




bool StartNetwork(ProcParams_s *ProcParams, NetworkParams_s *NetPars, bool AllwaysTermEnabled);
void DoNetwork(ProcParams_s *ProcParams, NetworkParams_s *NetPars, NetQueue_s *NetQ);
bool SendToNetwork(ProcParams_s *ProcParams, NetworkParams_s *NetPars, void *Data, size_t Len, CustAcknowledge_s *CustAckInfo_p);
void CloseNetwork(ProcParams_s *ProcParams, NetworkParams_s *NetPars, LogLevel_e LogLevel);

void LogStartOfParking(ProcParams_s *ProcParams, CustAcknowledge_s *CustAckInfo, GPS_Cords_s *Cords);
void LogEnfOfParking(ProcParams_s *ProcParams, CustAcknowledge_s *CustAckInfo, CustAcknowledge_s *LastCustAckInfo, GPS_Cords_s *Cords);

uint32_t GenParkTime();
uint32_t GenParkWaitTime();
void GenParkMessage(char Buffer[], size_t NumBytes, time_t StartTime, time_t Duration, bool ParkingStartEnd);


void NetworkProc(ProcParams_s *ProcParams)
 {
  NetworkParams_s NetworkParams = {0};
  NetQueue_s NetQueue = {0};

  InitNetQueue(&NetQueue.mq, QUEUE_RECEIVE_E);

  DoNetwork(ProcParams, &NetworkParams, &NetQueue);

  CloseNetQueue(&NetQueue.mq);
 }




bool StartNetwork(ProcParams_s *ProcParams, NetworkParams_s *NetPars, bool AllwaysTermEnabled)
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
     {
      char buf[50];
      snprintf(buf, sizeof(buf), "%sSocket creation error%s", (StdErrNoPiping ? ResultColors[E_FAIL] : ""), (StdErrNoPiping ? TermColorsReset : ""));
      perror(buf);
     }
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
      fprintf(stderr, "%sCouldn't detect dynamic address from DHCP name %s.%s\n\r", (StdErrNoPiping ? ResultColors[E_FAIL] : ""), DHCPName, (StdErrNoPiping ? TermColorsReset : ""));
     }
   }
  
  if((UseDHCP) && (host != NULL))  /* DHCP configuration was enaled and IP address was detected successfully from DHCP name. */
   {
    fprintf(stdout, "Loading DHCP address...\n\r");
    memcpy(&NetPars->server_addr.sin_addr, host->h_addr_list[0], MIN(host->h_length, (int)sizeof(NetPars->server_addr.sin_addr)));
   }
  else  /* Was set the manual IP or IP address wasn't detected from DHCP server. */
   {
    DestIP = GetDestinAddr();  /* Loading IP address from configuration ".ini" file   */
    if(AllwaysTermEnabled)
     {
      fprintf(stdout, "Loading Static IP address...\n\r");
     }
    /* Convert IPv4 address from text to binary format */
    if (inet_pton(AF_INET, DestIP, &NetPars->server_addr.sin_addr) <= 0)   //  if (inet_pton(AF_INET, DESTIN_IP, &NetPars->server_addr.sin_addr) <= 0) 
     {
      if((AllwaysTermEnabled) || (last_errno != errno))
       {
        char buf[50];
        snprintf(buf, sizeof(buf), "%sInvalid address or Address not supported%s", (StdErrNoPiping ? ResultColors[E_FAIL] : ""), (StdErrNoPiping ? TermColorsReset : ""));
        perror(buf);
        last_errno = errno;
       }
      close(NetPars->sock_fd);
      return false;
     }
   }

   //inet_ntop(AF_INET, host->h_addr_list, IP_Addr, host->h_length);

  uint8_t ad[4];
  memcpy(ad, &NetPars->server_addr.sin_addr, 4);
  if(AllwaysTermEnabled)
   {
    printf("Trying to connect to the address %d.%d.%d.%d with port %d ...\n\r", ad[0], ad[1], ad[2], ad[3], DestinPort);
   }

  /* Connect to the server */
  if (connect(NetPars->sock_fd, (struct sockaddr *)&NetPars->server_addr, sizeof(NetPars->server_addr)) < 0) 
   {
    if((AllwaysTermEnabled) || (last_errno != errno))
     {
      char buf[50];
      snprintf(buf, sizeof(buf), "%sConnection Failed%s", (StdErrNoPiping ? ResultColors[E_FAIL] : ""), (StdErrNoPiping ? TermColorsReset : ""));
      perror(buf);
      fprintf(stderr, "%sRecheck server%s\n\r", (StdErrNoPiping ? ResultColors[E_WARNING] : ""), (StdErrNoPiping ? TermColorsReset : ""));

      last_errno = errno;
     }
    close(NetPars->sock_fd);
    return false;
   }
  printf("%sConnected successfully to the server.%s\n\r", (StdOutNoPiping ? ResultColors[E_CORRECT] : ""), (StdOutNoPiping ? TermColorsReset : ""));  /* Must be printed anyway */
  char buf[100];
  snprintf(buf, sizeof(buf), "Connected successfully to the server %d.%d.%d.%d:%d.", ad[0], ad[1], ad[2], ad[3], DestinPort);
  LogEvent(&ProcParams->TskContShqs, MakeLogMessage(E_LOG_EVENT, ProcParams->ProcName, buf));
  return true;
 }

bool SendToNetwork(ProcParams_s *ProcParams, NetworkParams_s *NetPars, void *Data, size_t Len, CustAcknowledge_s *CustAckInfo_p)    /*  Send ▬▬▬▶ Network   ⸺▶    Wait for response   ⸺▶   Network ▬▬▬▶ Receive */
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
    char buf[50];
    snprintf(buf, sizeof(buf), "%sSend failed.%s", (StdErrNoPiping ? ResultColors[E_FAIL] : ""), (StdErrNoPiping ? TermColorsReset : ""));
    perror(buf);
    fprintf(stderr, "%serrno %d%s\n\r", (StdErrNoPiping ? ResultColors[E_FAIL] : ""), errno, (StdErrNoPiping ? TermColorsReset : ""));
    //EPIPE;
    LogEvent(&ProcParams->TskContShqs, MakeLogMessage(E_LOG_FAIL, ProcParams->ProcName, "Send failed."));
   }
  else
   {
#if !defined(__arm__)
    fprintf(stdout, "%sWere sent %ld bytes to the network.%s\n\r", (StdOutNoPiping ? ResultColors[E_SUCCESS] : ""), SentDataSize, (StdOutNoPiping ? TermColorsReset : ""));
#else
    fprintf(stdout, "%sWere sent %d bytes to the network.%s\n\r", (StdOutNoPiping ? ResultColors[E_SUCCESS] : ""), SentDataSize, (StdOutNoPiping ? TermColorsReset : ""));
#endif

    FreeData(&DataForSending);
  
    /* Receive data back from the server */
    bytes_read = recv(NetPars->sock_fd, buffer, BUFFER_SIZE - 1, 0);
    Result = (bytes_read >= 0);
    if (bytes_read < 0) 
     {
      char buf[50];
      snprintf(buf, sizeof(buf), "%sReceive failed.%s", (StdErrNoPiping ? ResultColors[E_FAIL] : ""), (StdErrNoPiping ? TermColorsReset : ""));
      perror(buf);
     } 
    else 
     if (bytes_read == 0) 
      {
       printf("%sServer closed the connection.%s\n\r", (StdOutNoPiping ? ResultColors[E_FAIL] : ""), (StdOutNoPiping ? TermColorsReset : ""));
       Result = false;
      } 
     else 
      {
       buffer[bytes_read] = '\0'; /* Null-terminate the received string */
  
#if !defined(__arm__)
       printf("Server responded data contains %ld bytes.\n\r", bytes_read);
#else
       printf("Server responded data contains %d bytes.\n\r", bytes_read);
#endif
       DecodeResult = DecodeNetData((uint8_t*)buffer, bytes_read, (uint8_t *)&CustAckInfo);
       if(DecodeResult)
        {
         printf("The vehicle is parked in: %s%s%s   (ID: %d)\n\r", (StdOutNoPiping ?  CITYNAME_COLOR : ""), CustAckInfo.City_Name, (StdOutNoPiping ?  TermColorsReset : ""), CustAckInfo.City_ID);
         CreateVehIDFormated(buffer, sizeof(buffer), CustAckInfo.Vechicle_ID, StdOutNoPiping);
         printf("Vehicle ID: %s%s\n\r", buffer, (StdErrNoPiping ? TermColorsReset : ""));
         ConvertTime(&CustAckInfo.ParkingStartTime, timedurbuf, sizeof(timedurbuf), E_CAL_FORMAT);
         printf("Parking started at: %s\n\r", timedurbuf);
         ConvertTime(&CustAckInfo.ParkingDurationTime, timedurbuf, sizeof(timedurbuf), E_DUR_FORMAT);
         ConvertPrice(CustAckInfo.AccumulatedPrice, buffer, sizeof(buffer), E_ACC_FORMAT, StdOutNoPiping);
         printf("Parking duration: %s   price: %s\n\r", timedurbuf, buffer);
         if(CustAckInfo_p != NULL)
         *CustAckInfo_p = CustAckInfo;
        }
       else
        {
         printf("%sError in response.%s\n\r", (StdErrNoPiping ? ResultColors[E_FAIL] : ""), (StdErrNoPiping ? TermColorsReset : ""));
         LogEvent(&ProcParams->TskContShqs, MakeLogMessage(E_LOG_ERROR, ProcParams->ProcName, "Error in response."));
        }
      }
   }
  return Result;
 }

void DoNetwork(ProcParams_s *ProcParams, NetworkParams_s *NetPars, NetQueue_s *NetQ)   /*  Queue  ▬▬▬▶ Network */
 {
  // bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  // bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  bool Connected = false, LastConnected = false;
  bool PrintTermOnConnection = true;
  unsigned int prio;
  char buffer[BUFFER_SIZE] = {0};
  struct timespec ts;
  GPS_Cords_s Cords;
  CustAcknowledge_s CustAckInfo = {0}, LastCustAckInfo = {0};
  uint32_t ParkTime = 0;  /* The time of start or end parking*/
  uint32_t ReqParkTime = 0;
  bool TimeTaken;

  srand(time(NULL));

  while((getppid() != 1) && (get_flag((TskContShmData_s*)ProcParams->TskContShms.p_shm, PROC_NETWORK_E) != true))
   {
    TimeTaken = (clock_gettime(CLOCK_REALTIME, &ts) != -1);

    if((!Connected))
     {
      if((ParkTime == 0) || !TimeTaken || (ts.tv_sec - ParkTime > ReqParkTime)) /* Attention !!! This condition is based on the conditional short-circuit and subconditions cannot be changedplaces. */
       {
        Connected = StartNetwork(ProcParams, NetPars, PrintTermOnConnection);
        PrintTermOnConnection = false;
       }
      if(Connected && TimeTaken)
       {
        ParkTime = ts.tv_sec;
        ReqParkTime = GenParkTime();
        GenParkMessage(buffer, sizeof(buffer), ts.tv_sec, ReqParkTime, false);
        printf("%s\n\r",buffer);
       }
     }

    if(Connected)
     {
      if((ParkTime > 0) && (ReqParkTime >= MIN_PARK_TIME) && TimeTaken && (ts.tv_sec - ParkTime > ReqParkTime)) /* Attention !!! This condition is based on the conditional short-circuit and subconditions cannot be changedplaces. */
       {  /* Parking timeout */
        CloseNetwork(ProcParams, NetPars, E_LOG_EVENT);
        Connected = false;
        ParkTime = ts.tv_sec;
        ReqParkTime = GenParkWaitTime();
        GenParkMessage(buffer, sizeof(buffer), ts.tv_sec, ReqParkTime, true);
        printf("%s\n\r",buffer);
       }
     }

    if (clock_gettime(CLOCK_REALTIME, &ts) != -1) 
     {
      ++ts.tv_sec;
     }

    /* Block until a message is received */
    ssize_t bytes_read = mq_timedreceive(NetQ->mq, buffer, MAX_SIZE, &prio, &ts);
    if (bytes_read >= 0) 
     {

#if !defined(__arm__)  /* For computer compiled program 64bit */
      printf("Were forwarded from the queue to the network %ld bytes.\n\r", bytes_read);
#else                  /* For BeagleBone compiled program 32bit */
      printf("Were forwarded from the queue to the network %d bytes.\n\r", bytes_read);
#endif
      
      PrintTermOnConnection = true;
      if(Connected)
       {
        Connected = SendToNetwork(ProcParams, NetPars, buffer, bytes_read, &CustAckInfo);
        if(!Connected)  /*Emergency disconnection detected. */
         {
          CloseNetwork(ProcParams, NetPars, E_LOG_FAIL);
          ParkTime = ReqParkTime = 0; /* Resetting parking timers. */
         }
       }
    
      if(Connected != LastConnected)
       {
        if(Connected)  /* On Connect*/
         {
          Cords = ((Customer_s*)buffer)->Cords;
          LastCustAckInfo = CustAckInfo;
          LogStartOfParking(ProcParams, &CustAckInfo, &Cords);
         }
        else  /* On Disconnect*/
         {
          LogEnfOfParking(ProcParams, &CustAckInfo, &LastCustAckInfo, &Cords);          
         }
        LastConnected = Connected;
       }

     } 
    
   }

  CloseNetwork(ProcParams, NetPars, E_LOG_MESSAGE);
  LogEnfOfParking(ProcParams, &CustAckInfo, &LastCustAckInfo, &Cords);          

 }

void CloseNetwork(ProcParams_s *ProcParams, NetworkParams_s *NetPars, LogLevel_e LogLevel)
 {
  // bool StdErrNoPiping = isatty(STDERR_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  bool StdOutNoPiping = isatty(STDOUT_FILENO); /* Checking if the output is not redirected to any other program or file to decide if to use colors or not. */
  
  /* Close the socket connection */
  close(NetPars->sock_fd);
  if(LogLevel == E_LOG_FAIL)
   {
    printf("%sDisconnected from server. Communication failed.%s\n\r", (StdOutNoPiping ? ResultColors[E_FAIL] : ""), (StdOutNoPiping ? TermColorsReset : ""));
    LogEvent(&ProcParams->TskContShqs, MakeLogMessage(E_LOG_EVENT, ProcParams->ProcName, "Disconnected from server. Communication Failed."));
   }
  else
   {
    Error_Results_e ErRes = ((LogLevel == E_LOG_EVENT) ? E_SUCCESS : E_PROBLEM);
    printf("%sDisconnected from server.%s\n\r", (StdOutNoPiping ? ResultColors[ErRes] : ""), (StdOutNoPiping ? TermColorsReset : ""));  
    LogEvent(&ProcParams->TskContShqs, MakeLogMessage(E_LOG_EVENT, ProcParams->ProcName, "Disconnected from server."));
   }
  // printf("%sDisconnected from server.%s\n\r", (StdOutNoPiping ? ResultColors[E_PROBLEM] : ""), (StdOutNoPiping ? TermColorsReset : ""));
  // LogEvent(&ProcParams->TskContShqs, MakeLogMessage(E_LOG_EVENT, ProcParams->ProcName, "Disconnected from server."));
 }


void LogStartOfParking(ProcParams_s *ProcParams, CustAcknowledge_s *CustAckInfo, GPS_Cords_s *Cords)
 {
  char buffer[BUFFER_SIZE] = {0}, timebuf1[50], cordsbuf[50], pricebuf[30];

  ConvertTime(&CustAckInfo->ParkingStartTime, timebuf1, sizeof(timebuf1), E_DBS_FORMAT);
  ConvertPrice(CustAckInfo->PricePerHour, pricebuf, sizeof(pricebuf), E_PPH_FULL_FORMAT, false);
  CordsToString(cordsbuf, sizeof(cordsbuf), *Cords);
  snprintf(buffer, sizeof(buffer), "The parking was started at: %s. GPS: %s. Detected city: %s ID: %d OSDID: %d. Park Price: %s", timebuf1, cordsbuf, CustAckInfo->City_Name, CustAckInfo->City_ID, CustAckInfo->OSM_ID, pricebuf);
  LogEvent(&ProcParams->TskContShqs, MakeLogMessage(E_LOG_EVENT, ProcParams->ProcName, buffer));

 }

void LogEnfOfParking(ProcParams_s *ProcParams, CustAcknowledge_s *CustAckInfo, CustAcknowledge_s *LastCustAckInfo, GPS_Cords_s *Cords)
 {
  char buffer[BUFFER_SIZE] = {0}, timebuf1[50], timebuf2[50], durtimebuf[50], cordsbuf[50], pricebuf[30], acpricebuf[30];

  LastCustAckInfo->ParkingEndTime = CustAckInfo->ParkingEndTime;
  LastCustAckInfo->AccumulatedPrice = CustAckInfo->AccumulatedPrice;
  LastCustAckInfo->ParkingDurationTime = CustAckInfo->ParkingDurationTime;

  ConvertTime(&LastCustAckInfo->ParkingStartTime, timebuf1, sizeof(timebuf1), E_DBS_FORMAT);
  ConvertTime(&CustAckInfo->ParkingEndTime, timebuf2, sizeof(timebuf2), E_DBS_FORMAT);
  ConvertTime(&CustAckInfo->ParkingDurationTime, durtimebuf, sizeof(durtimebuf), E_DUR_FORMAT);
  ConvertPrice(LastCustAckInfo->PricePerHour, pricebuf, sizeof(pricebuf), E_PPH_FORMAT, false);
  ConvertPrice(CustAckInfo->AccumulatedPrice, acpricebuf, sizeof(acpricebuf), E_ACC_FORMAT, false);
  CordsToString(cordsbuf, sizeof(cordsbuf), *Cords);

  snprintf(buffer, sizeof(buffer), "The parking was finished. GPS: %s  City: %s ID: %d OSDID: %d. Period: %s - %s; Duration %s Park price %s, For paying %s.", cordsbuf, LastCustAckInfo->City_Name, LastCustAckInfo->City_ID, LastCustAckInfo->OSM_ID, timebuf1, timebuf2, durtimebuf, pricebuf, acpricebuf);
  LogEvent(&ProcParams->TskContShqs, MakeLogMessage(E_LOG_EVENT, ProcParams->ProcName, buffer));
 }


void InitNetQueue(mqd_t *mq, QueueDirection_e SendReceive)
 {
  InitQueue(mq, SendReceive, QUEUE_NAME, MAX_SIZE);
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
    printf("Message to network queue was sent successfully. %ld bytes sent.\n\r", Len);
#else
    printf("Message sent successfully. %d bytes sent.\n\r", Len);
#endif
   }
 }

void CloseNetQueue(mqd_t *mq)
 {
  CloseQueue(mq, QUEUE_NAME);
 }

uint32_t GenParkTime()
 {
  return GenRandNumber(MIN_PARK_TIME, GetMaxParkTime());
 }

uint32_t GenParkWaitTime()
 {
  return GenRandNumber(MIN_PARK_WAIT_TIME, GetMaxParkWaitTime());
 }

void GenParkMessage(char Buffer[], size_t NumBytes, time_t StartTime, time_t Duration, bool ParkingStartEnd)
 {
  char timebuf1[100], timebuf2[100], timebuf3[100];
  time_t et = StartTime + Duration;
  ConvertTime(&StartTime  , timebuf1, sizeof(timebuf1), E_CAL_FORMAT);
  ConvertTime(&et         , timebuf2, sizeof(timebuf2), E_CAL_FORMAT);
  et = Duration;
  ConvertTime(&et         , timebuf3, sizeof(timebuf2), E_DUR_FORMAT);

  if(ParkingStartEnd == false)
   snprintf(Buffer, NumBytes, "The parking started at: %s and will be finished at %s after %s", timebuf1, timebuf2, timebuf3);
  else
   snprintf(Buffer, NumBytes, "The parking finished at: %s and will be started at %s after %s", timebuf1, timebuf2, timebuf3);

 }

