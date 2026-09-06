#ifndef ____Network_h__
#define ____Network_h__

/*======================================================================================================================*/

#include <mqueue.h>
#include <arpa/inet.h>
#include "CommonData.h"
#include "Processes.h"


/*======================================================================================================================*/

#define NETW_PROC_NAME     (char *)"Network"     /* Network process name*/

/*======================================================================================================================*/

/*----------------------------------------------------------------------------------------------------------------------*/
/* The structure containing parameters for using the network queue.                                                     */
typedef struct NetQueue_s
 {
  mqd_t mq;
 }
NetQueue_s;

/*----------------------------------------------------------------------------------------------------------------------*/
/* The structure containing parameters for connection to the Parking Managing Server.                                   */
typedef struct NetworkParams_s
 {
  int sock_fd;
  struct sockaddr_in server_addr;
 }
NetworkParams_s;


/*======================================================================================================================*/

/**
 * @brief Main Process' operating procedure.
 * 
 * @code
 * void NetworkProc(ProcParams_s *ProcParams);
 * @code
 * 
 * @param ProcParams The strucutre containing data required for process work.
 */
void NetworkProc(ProcParams_s *ProcParams);

/*======================================================================================================================*/

/**
 * @brief Sends Message to the network queue for forwarding it then to the network.  Process ▬▬▬▶ Queue
 * 
 * @code
 * void SendMessageToNetwork(NetQueue_s *NetQ, void *Data, size_t Len);      Process ▬▬▬▶ Queue  
 * @code
 * 
 * @param NetQ  The given pointer to the network queue for sending messages.
 * 
 * @param Data  The given data for sending in the message.
 * 
 * @param Len   The given size of the data for sending the message.
 */
void SendMessageToNetwork(NetQueue_s *NetQ, void *Data, size_t Len);    /*  Process ▬▬▬▶ Queue  */

/*======================================================================================================================*/

/**
 * @brief Generates the queue name for sending messages between I2C and Network processes.
 * 
 * @code
 * void GenerateNetQueueName(char Name[], size_t Size);
 * @code
 * 
 * @param Name  The returned generated name of the queue.
 * 
 * @param Size  The maximum possible length of the name. If the "Size" is smaller than required the part of the data will be lost.
 */
void GenerateNetQueueName(char Name[], size_t Size);

/**
 * @brief Initilizes Network Queue.
 * 
 * @code
 * void InitNetQueue(mqd_t *mq, QueueDirection_e SendReceive, char const QueueName[]);
 * @code
 * 
 * @param mq The given pointer to the message queue for initialization.
 * 
 * @param SendReceive The queue direction. From the Network side it must be receiving and from the I2C side - for sending.
 * 
 * @param QueueName The given name of the queue which must be initialized.
 */
void InitNetQueue(mqd_t *mq, QueueDirection_e SendReceive, char const QueueName[]);

/**
 * @brief Closes Network Queue.
 * 
 * @code
 * void CloseNetQueue(mqd_t *mq, char const QueueName[]);
 * @code
 * 
 * @param mq The given pointer to the message queue for deinitialization.
 * 
 * @param QueueName The given name of the queue which must be initialized.
 */
void CloseNetQueue(mqd_t *mq, char const QueueName[]);

/*======================================================================================================================*/



#endif  //  ____Network_h__

