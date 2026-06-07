#include "Network.hpp"
#include "main.hpp"
#include "Errors.hpp"
#include "Processes.hpp"

#include <semaphore.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define PORT 8080  // Will be moved in the future to the "CommonLib.h" file.
#define BUFFER_SIZE 1024




#define NETW_PROC_NAME     (char *)"Network"     /* Network process name*/

class Network_c: public Process_c
 {
    int serverSocket = 0, newSocket = 0;  
    sockaddr_in address = {{0},0,{0},{0}};
    int addrlen = 0;
  public:
    Network_c(char ProcName[], key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType);
    virtual ~Network_c();
    virtual void DoMainProg();
 };


void NetworkProc(key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType)
 {
  Network_c Netw_Process(NETW_PROC_NAME, sh_mem_key, sem_name, ProcType);
  Netw_Process.OnRunProcess();
 }





void handleClient(int clientSocket) 
 {
  char buffer[BUFFER_SIZE];
  std::cout << "Handling client in thread ID: " << std::this_thread::get_id() << "\n\r";
  // Communication loop
  while (true) 
   {
    memset(buffer, 0, BUFFER_SIZE);
    int bytesRead = read(clientSocket, buffer, BUFFER_SIZE);
    
    if (bytesRead <= 0) 
     {
      std::cout << "Client disconnected or error.\n\r";
      break;
     }
    std::cout << "Received: " << buffer;
    // Echo response back to client
    write(clientSocket, buffer, strlen(buffer));
   }
  close(clientSocket);
 }





Network_c::Network_c(char ProcName[], key_t sh_mem_key, const char sem_name[], ProcTypeID_e ProcType)
 :Process_c(ProcName, sh_mem_key, sem_name, ProcType)
 {
  int opt = 1;
  timeval timeout;
  timeout.tv_sec = 5;
  timeout.tv_usec = 0;

  addrlen = sizeof(address);

  // 1. Create the server socket
  if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
   {
    //std::cerr << "Socket creation failed\n\r";
    perr() << "Socket creation failed";
    //exit(EXIT_FAILURE);
    return;
   }

  // 2. Attach socket to the port (prevents 'Address already in use' errors)
  if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) 
   {
    //std::cerr << "setsockopt failed\n\r";
    perr() << "setsockopt failed";
    MakeExit();
    //exit(EXIT_FAILURE);
    return;
   }


  if (setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) 
   {
    //std::cerr << "Setting SO_RCVTIMEO failed\n\r";
    perr() << "Setting SO_RCVTIMEO failed";
    // Handle error or close socket
   }


  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  // 3. Bind the socket
  if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0) 
   {
    //std::cerr << "Bind failed\n\r";
    perr() << "Bind failed";
    MakeExit();
    //exit(EXIT_FAILURE);
    return;
   }

  // 4. Start listening for incoming connections
  if (listen(serverSocket, 3) < 0) 
   {
    //std::cerr << "Listen failed\n\r";
    perr() << "Listen failed";
    MakeExit();
    //exit(EXIT_FAILURE);
    return;
   }
  std::cout << "Server listening on port " << PORT << "...\n\r";
 }

void Network_c::DoMainProg()
 {
  //Process_c::DoMainProg();
  //std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\r";
  // 5. Accept connections in a loop
  if ((newSocket = accept(serverSocket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) 
   {
    // //std::cerr << "Accept failed\n\r";
    // perr() << "Accept failed";
    // MakeExit();
    // //exit(EXIT_FAILURE);
    // return;
   }
  else
   {
    std::cout << "New connection accepted.\n\r";
    // 6. Spawn a new thread to handle the client
    std::thread t(handleClient, newSocket);
    t.detach(); // Detach the thread so it runs independently
   }
  //std::cout << "------------------------------------------------------------\n\r";
 }
 
Network_c::~Network_c()
 {
  if(serverSocket != 0)
   {
    close(serverSocket);
    serverSocket = 0;
    std::cout << "The socket was closed. \n\r";
   }
  //return 0;
 }


