#include <unistd.h>
#include <stdio.h>
#include <signal.h>
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

  // Se inicializa un loop para recibir todo tipo de paquetes y tomar una acción al respecto
  while (1) {
    int msg_code = client_receive_id(server_socket);

    // Mensajes singulares de servidor a cliente
    if (msg_code == 0) {
      char * message = client_receive_payload(server_socket);
      // printf("El servidor dice: %s\n", message);
      printf("%s\n", message);
      free(message);
    }

    else if (msg_code == 1) {
      // Server envía al Menú de Inicio
      char * message = client_receive_payload(server_socket);
      printf("%s", message);
      free(message);

      char *initial_menu = 
      "\n*** DCCorreo ***\n"
      "----------------\n"
      "[1] Registrarse\n"
      "[2] Iniciar sesión\n"
      "[0] Salir\n"
      "\nIndique su opción (1, 2 o 0): ";

      printf("%s", initial_menu);
      int option = getchar() - '0';
      getchar();
      printf("\n");

      if (option == 0) {  // Terminar ejecución
        printf("Cerrando la app...\n");

        char *response = "";
        client_send_message(server_socket, 0, response);

        // clean memory

        raise(SIGINT);
      }

      else if (option == 1) { // Register, code: 1
        printf("Ingrese el nombre de usuario con el que desea registrarse: ");
        char *username = get_input();
        printf("\n");

        client_send_message(server_socket, 1, username);
      }
      
      else if (option == 2) { // Login, code: 2
        printf("Ingrese su nombre de usuario: ");
        char *username = get_input();
        printf("\n");

        client_send_message(server_socket, 2, username);
      }
    }

    else if (msg_code == 2) {
      // Bandeja de entrada
      char * message = client_receive_payload(server_socket);
      printf("%s", message);
      free(message);

      char * main_menu =
      "\n*** Bandeja de Entrada ***\n"
      "--------------------------\n"
      "[1] Enviar correo electrónico\n"
      "[2] Ver correos enviados y recibidos\n"
      "[3] Ver usuarios conectados y desconectados\n"
      "[0] Salir de la aplicación\n"
      "\nIndique su opción (1, 2, 3 o 0): ";

      printf("%s", main_menu);
      int option = getchar() - '0';
      getchar(); //Para capturar el "enter" que queda en el buffer de entrada stdin
      char * response = "";
      printf("\n");

      if (option == 0) {
        // Salir aplicación
        printf("Se cierra la app\n");
        client_send_message(server_socket, 0, response);
        raise(SIGINT);
      }
      else if (option == 1) {
        // Enviar correo
        char * message_commands =
        "\n*** Enviar Mensaje ***\n"
        "----------------------\n"
        "Utilice alguno de los siguientes comandos:\n"
        "Mensaje inmediato de texto:\t it,<destinatario>,<contenido>\n"
        "Mensaje inmediato de archivo:\t if,<destinatario>,<nombre_archivo>.<ext>\n"
        "Mensaje programado de texto:\t pt,<destinatario>,<contenido>,<s>\n"
        "Mensaje programado de archivo:\t pf,<destinatario>,<nombre_archivo>.<ext>,<s>\n\n";

        char input[256];
        printf("%s", message_commands);
        printf("Escriba el comando apropiado para su mensaje: ");
        fgets(input, sizeof(input), stdin);
        printf("\n");

        // char command[3], recipient[11], content[256];
        // unsigned int seconds;

        char *response;
        response = strdup(input);
        client_send_message(server_socket, 3, response);

        // if (sscanf(input, "%2[^,],%10[^,],%255[^,],%u", command, recipient, content, &seconds) == 4) {
        //   if (strcmp(command, "pt") == 0 || strcmp(command, "pf") == 0) {
        //     // Se trata de un mensaje programado
        //     printf("[%s] Programando un mensaje en %us desde ahora...\n", command, seconds);
        //     char *response;
        //     response = strdup(input);
        //     // strcat(response, input);
        //     int response_code;
        //     if (strcmp(command, "pt") == 0) {
        //       // code 5
        //       response_code = 3;
        //     }
        //     else {
        //       response_code = 3;
        //     }
        //     client_send_message(server_socket, response_code, response);
        //   }
        //   else {
        //     printf("Comando inválido\n");
        //   }
        // }
        // else if (sscanf(input, "%2[^,],%10[^,],%255[^,]", command, recipient, content) == 3) {
        //   if (strcmp(command, "it") == 0 || strcmp(command, "if") == 0) {
        //     // Se trata de un mensaje inmediato
        //     printf("Enviando un mensaje inmediato...\n");
        //     char *response;
        //     response = strdup(input);
        //     // strcat(response, input);
        //     int response_code;
        //     if (strcmp(command, "it") == 0) {
        //       // code 3
        //       response_code = 3;
        //     }
        //     else {
        //       // code 4
        //       response_code = 3;
        //     }
        //     client_send_message(server_socket, response_code, response);
        //   }
        //   else {
        //     printf("Comando inválido\n");
        //   }
        // }
        // else {
        //   printf("El formato del comando es inválido\n");
        // }
      }
      else if (option == 2) {
        // Ver correos enviados y recibidos
        ;
      }
      else if (option == 3) {
        // Usuarios conectados y desconectados
        char * response = "";
        client_send_message(server_socket, 8, response);
        printf("\n");
      }
    }

    // else if (msg_code == 3) { // Enviar mensaje
    //   char *message = client_receive_payload(server_socket);
    //   printf("%s", message);
    //   free(message);

    //   char * message_commands =
    //   "\n*** Enviar Mensaje ***\n"
    //   "----------------------\n"
    //   "Utilice alguno de los siguientes comandos:\n"
    //   "Mensaje de texto inmediato:\t it,<destinatario>,<contenido>\n"
    //   "Mensaje de archivo inmediato:\t if,<destinatario>,<nombre_archivo>.<ext>\n"
    //   "Mensaje de texto programado:\t pt,<destinatario>,<contenido>,<s>\n"
    //   "Mensaje de archivo programado:\t if,<destinatario>,<nombre_archivo>.<ext>,<s>\n\n";

    //   char input[512];
    //   printf("%s", message_commands);
    //   printf("Escriba el comando apropiado para su mensaje: ");
    //   fgets(input, sizeof(input), stdin);
    //   printf("\n");

    //   char command[3], recipient[11], content[256];
    //   unsigned int seconds;

    //   if (sscanf(input, "%2[^,],%10[^,],%255[^,],%u", command, recipient, content, &seconds) == 4) {
    //     if (strcmp(command, "pt") == 0 || strcmp(command, "pf") == 0) {
    //       // Se trata de un mensaje programado
    //       printf("[%s] Programando un mensaje en %us desde ahora...\n", command, seconds);
    //       char *response = "";
    //       strcat(response, input);
    //       int response_code;
    //       if (strcmp(command, "pt") == 0) {
    //         // code 5
    //         response_code = 5;
    //       }
    //       else {
    //         response_code = 6;
    //       }
    //       client_send_message(server_socket, response_code, response);
    //     }
    //     else {
    //       printf("Comando inválido\n");
    //     }
    //   }
    //   else if (sscanf(input, "%2[^,],%10[^,],%255[^,]", command, recipient, content) == 3) {
    //     if (strcmp(command, "it") == 0 || strcmp(command, "if") == 0) {
    //       // Se trata de un mensaje inmediato
    //       printf("Enviando un mensaje inmediato...\n");
    //     }
    //     else {
    //       printf("Comando inválido\n");
    //     }
    //   }
    //   else {
    //     printf("El formato del comando es inválido\n");
    //   }
    // }

    // // Mensaje de error
    // else if (msg_code == 4) {
    //   char * message = client_receive_payload(server_socket);
    //   printf("El servidor dice: %s\n", message);
    //   // free(message);
    //   // printf("Maximum number of connections reached. Please try again later.\n");
    //   break;
    // }

    // printf("------------------\n");
  }

  // Se cierra el socket
  close(server_socket);
  free(IP);

  return 0;
}
