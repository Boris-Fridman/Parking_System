#ifndef ____Network_h__
#define ____Network_h__

#include <mqueue.h>
#include <arpa/inet.h>

typedef enum QueueDirection_e
 {
  QUEUE_SEND_E,
  QUEUE_RECEIVE_E
 }
QueueDirection_e;

typedef struct NetQueue_s
 {
  mqd_t mq;
 }
NetQueue_s;
typedef struct NetworkParams_s
 {
  int sock_fd;
  struct sockaddr_in server_addr;
 }
NetworkParams_s;



void NetworkProc();

void SendMessageToNetwork(NetQueue_s *NetQ, void *Data, size_t Len);

void InitNetQueue(NetQueue_s *NetQ, QueueDirection_e SendReceive);
void CloseNetQueue(NetQueue_s *NetQ);





#endif  //  ____Network_h__

