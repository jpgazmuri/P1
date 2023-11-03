#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include "comunication.h"
// #include "../others/users.h"
// #pragma once

typedef struct user {
  char *username;
  int is_logged;  // 0 logged out, 1 logged in
} User;

typedef struct users_info {
  int num_users;
  User * users_list[6];
} UsersInfo;

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
  UsersInfo *users_info;
  User *user;
} Client;

typedef struct client_info {
  int client_socket;
  int client_number;
  int *client_sockets;
  int *num_clients;
} ClientInfo;


int check_username(char username, UsersInfo *users_info);
PlayersInfo * prepare_sockets_and_get_clients(char * IP, int port);
ClientsInfo * connections_handler(char * IP, int port, UsersInfo *users_info);
void * client_handler(void *arg);