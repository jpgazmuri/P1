#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "../sockets/comunication.h"
// #include "../others/users.h"
#include "../sockets/conection.h"

char * revert(char * message){
  //Se invierte el mensaje
  
  int len = strlen(message) + 1;
  char * response = malloc(len);

  for (int i = 0; i < len-1; i++)
  {
    response[i] = message[len-2-i];
  }
  response[len-1] = '\0';
  return response;
}

int main(int argc, char *argv[]){
  // Se extrae el IP y PORT de los args, si no se entregan se utilizan valores default
  char * IP = "0.0.0.0";
  int PORT = 8080;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
      IP = argv[i + 1];
    } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
      PORT = atoi(argv[i + 1]);
    }
  }

  // Se crea el servidor y se obtienen los sockets de ambos clientes.
  printf("Creando servidor...\n");
  printf("Selected IP: %s\n", IP);
  printf("Selected Port: %d\n", PORT);
  // PlayersInfo * players_info = prepare_sockets_and_get_clients(IP, PORT);

  UsersInfo *users_info = calloc(1, sizeof(UsersInfo));
  ClientsInfo *clients_info = calloc(1, sizeof(ClientsInfo));
  
  connections_handler(IP, PORT, clients_info, users_info);

  // Limpieza de memoria
  free(clients_info);
  free(users_info);

  return 0;
}
