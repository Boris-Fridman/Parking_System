#ifndef ____Network_h__
#define ____Network_h__

#include <mqueue.h>
#include <arpa/inet.h>
#include "CommonData.h"

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



void NetworkProc(key_t sh_mem_key, char sem_name[]);

void SendMessageToNetwork(NetQueue_s *NetQ, void *Data, size_t Len);

void InitNetQueue(mqd_t *mq, QueueDirection_e SendReceive);
void CloseNetQueue(mqd_t *mq);





#endif  //  ____Network_h__

