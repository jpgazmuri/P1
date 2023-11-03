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

typedef struct message {
  char *sender;
  char *recipient;
  int message_type; // Texto: 0, Archivo: 1
  char content[256];
  int is_downloaded; // Para texts, default 1
  // chunks, puede tomar 128 chunks de 256 bytes para un archivo de 32KB (tamaño max)
} Message;

typedef struct message_list {
  Message *message;
  struct message_list *next;
} MessageList;

typedef struct user {
  char *username;
  int is_logged;  // 0 logged out, 1 logged in
  MessageList *sent_messages;
  MessageList *received_messages;
} User;

typedef struct users_info {
  int num_users;
  User * users_list[6];
} UsersInfo;

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

// typedef struct client_info {
//   int client_socket;
//   int client_number;
//   int *client_sockets;
//   int *num_clients;
// } ClientInfo;


int check_username(char username, UsersInfo *users_info);
void connections_handler(char * IP, int port, ClientsInfo *clients, UsersInfo *users_info);
// ClientsInfo * connections_handler(char * IP, int port, UsersInfo *users_info);
void * client_handler(void *arg);
void * add_message_to_list(Message *message, MessageList *message_list);