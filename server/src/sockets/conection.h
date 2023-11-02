#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include "comunication.h"

typedef struct players_info{
  int socket_c1;
  int socket_c2;
  int socket_c3;
  int socket_c4;
} PlayersInfo;

typedef struct clients_info {
  int client_sockets[4];
  int num_clients;
  // char users[6][11];
} ClientsInfo;

typedef struct client {
  int client_socket;
  int client_number;
  ClientsInfo *clients_info;
} Client;

typedef struct client_info {
  int client_socket;
  int client_number;
  int *client_sockets;
  int *num_clients;
} ClientInfo;

// typedef struct users_info {
//   char users[6][11];
// } UsersInfo;


PlayersInfo * prepare_sockets_and_get_clients(char * IP, int port);
ClientsInfo * connections_handler(char * IP, int port);
void * client_handler(void *arg);