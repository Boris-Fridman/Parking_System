#include "Network.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include "CommonData.h"



void StartNetwork();
void DoNetwork();
void CloseNetwork();


void NetworkProc()
 {

  StartNetwork();
  DoNetwork();
  CloseNetwork();
   
 }



int sock_fd = 0;
struct sockaddr_in server_addr;
char *message = "Hello from the C Client!";
char buffer[BUFFER_SIZE] = {0};

void StartNetwork()
 {

  // 1. Create the socket
  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0) 
   {
    perror("Socket creation error");
    exit(EXIT_FAILURE);
   }

  // 2. Configure the server address structure
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(DESTIN_PORT);
  // Convert IPv4 address from text to binary format
  if (inet_pton(AF_INET, DESTIN_IP, &server_addr.sin_addr) <= 0) 
   {
    perror("Invalid address or Address not supported");
    close(sock_fd);
    exit(EXIT_FAILURE);
   }


  // 3. Connect to the server
  if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
   {
    perror("Connection Failed");
    close(sock_fd);
    exit(EXIT_FAILURE);
   }
  printf("Connected successfully to the server.\n");


 }

void DoNetwork()
 {
  // 4. Send data to the server
  if (send(sock_fd, message, strlen(message), 0) < 0) 
   {
    perror("Send failed");
    close(sock_fd);
    exit(EXIT_FAILURE);
   }
  printf("Message sent: %s\n", message);

  // 5. Receive data back from the server
  ssize_t bytes_read = recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);
  if (bytes_read < 0) 
   {
    perror("Receive failed");
   } 
  else if (bytes_read == 0) 
   {
    printf("Server closed the connection.\n");
   } 
  else 
   {
    buffer[bytes_read] = '\0'; // Null-terminate the received string
    printf("Server response: %s\n", buffer);
   }

 }

void CloseNetwork()
 {
  // 6. Close the socket connection
  close(sock_fd);
  //return 0;

 }
