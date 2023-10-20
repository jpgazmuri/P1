#include <unistd.h>
#include <stdio.h>
#include "../sockets/conection.h"
#include "../sockets/comunication.h"

char * get_input(){
  char * response = malloc(20);
  int pos=0;
  while (1){
    char c = getchar();
    if (c == '\n') break;
    response[pos] = c;
    pos++;
  }
  response[pos] = '\0';
  return response;
}


int main (int argc, char *argv[]){
  // Se obtiene la IP y el PORT donde está escuchando el servidor dada por los args, si no se entregan se usan valores default
  char * IP = "0.0.0.0";
  int PORT = 8080;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
      IP = argv[i + 1];
    } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
      PORT = atoi(argv[i + 1]);
    }
  }

  // Se prepara el socket
  printf("Conectando con el servidor...\n");
  printf("IP: %s\n", IP);
  printf("PORT: %d\n", PORT);
  int server_socket = prepare_socket(IP, PORT);
  printf("Conexión establecida\n");

  // Status de la session
  // int current_session = 0;
  // if (!current_session) {
  //   printf("\n*** DCCorreo ***\n");
  //   printf("----------------\n");
  //   printf("[1] Registrarse\n");
  //   printf("[2] Iniciar sesión\n");
  //   printf("[X] Salir\n");
  //   printf("\nIndique su opción (1, 2 o X): ");
  //   char * option = get_input();
  //   getchar();
  // }

  // Se inicializa un loop para recibir todo tipo de paquetes y tomar una acción al respecto
  while (1) {
    int msg_code = client_receive_id(server_socket);

    if (msg_code == 0) { // Servidor envía mensaje de conexión inicial
      char * message = client_receive_payload(server_socket);
      printf("El servidor dice: %s\n", message);
      free(message);
    }
    
    else if (msg_code == 1) { //Recibimos un mensaje del servidor
      char * message = client_receive_payload(server_socket);
      printf("El servidor dice: %s\n", message);
      free(message);

      printf("¿Qué desea hacer?\n   1)Enviar mensaje al servidor\n   2)Enviar mensaje al otro cliente\n");
      int option = getchar() - '0';
      getchar(); //Para capturar el "enter" que queda en el buffer de entrada stdin
      
      printf("Ingrese su mensaje: ");
      char * response = get_input();

      client_send_message(server_socket, option, response);
    }

    if (msg_code == 2) { //Recibimos un mensaje que proviene del otro cliente
      char * message = client_receive_payload(server_socket);
      printf("El otro cliente dice: %s\n", message);
      free(message);

      printf("¿Qué desea hacer?\n   1)Enviar mensaje al servidor\n   2)Enviar mensaje al otro cliente\n");
      int option = getchar() - '0';
      getchar(); //Para capturar el "enter" que queda en el buffer de entrada stdin
      
      printf("Ingrese su mensaje: ");
      char * response = get_input();

      client_send_message(server_socket, option, response);
    }
    printf("------------------\n");
  }

  // Se cierra el socket
  close(server_socket);
  free(IP);

  return 0;
}
