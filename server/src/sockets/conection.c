#include "conection.h"

#define MAX_CLIENTS 4

//LINKS REFERENCIAS:
//https://www.man7.org/linux/man-pages/man2/socket.2.html
//https://man7.org/linux/man-pages/man7/socket.7.html
//https://www.man7.org/linux/man-pages/man3/setsockopt.3p.html
//https://man7.org/linux/man-pages/man2/setsockopt.2.html
//https://linux.die.net/man/3/htons
//https://linux.die.net/man/3/inet_aton
//https://www.howtogeek.com/225487/what-is-the-difference-between-127.0.0.1-and-0.0.0.0/
//https://www.man7.org/linux/man-pages/man2/bind.2.html
//https://www.man7.org/linux/man-pages/man2/accept.2.html

// void *client_handler(void *arg) {
//   ClientInfo *client_info = (ClientInfo *)arg;
//   int client_socket = client_info->client_socket;
//   int client_number = client_info->client_number;

//   // Determine the client number based on the client socket
//   // for (int i = 0; i < 4; i++) {
//   //   if (client_sockets[i] == client_socket) {
//   //     client_number = i + 1;
//   //     break;
//   //   }
//   // }

//   // while (1) {
//   //   int msg_code = server_receive_id(client_socket);

//   //   if (msg_code == 1) { // Message from client to server
//   //     char *client_message = server_receive_payload(client_socket);
//   //     printf("Client %d says: %s\n", client_number, client_message);

//   //     // Send the inverted message back to the client
//   //     char *response = revert(client_message);
//   //     server_send_message(client_socket, 1, response);
//   //   } else if (msg_code == 2) { // Message from client to another client
//   //     char *client_message = server_receive_payload(client_socket);
//   //     printf("Server forwarding message from %d to %d: %s\n", client_number, ((client_number % 4) + 1), client_message);

//   //     // Send the message to the next client in the array
//   //     int next_client = (client_number % 4);
//   //     server_send_message(client_info->client_sockets[next_client], 2, client_message);
//   //   }
//   // }

//   // Handle disconnection and cleanup here
//   // ...
//   client_info->client_sockets[client_number - 1] = 0; // Mark the slot as empty
//   (*client_info->num_clients)--;
//   close(client_socket);
//   pthread_exit(NULL);
// }


PlayersInfo * prepare_sockets_and_get_clients(char * IP, int port){
  // Se define la estructura para almacenar info del socket del servidor al momento de su creación
  struct sockaddr_in server_addr;

  // Se solicita un socket al SO, que se usará para escuchar conexiones entrantes
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);

  // Se configura el socket a gusto (recomiendo fuertemente el REUSEPORT!)
  int opt = 1;
  int ret = setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

  // Se guardan el puerto e IP en la estructura antes definida
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  inet_aton(IP, &server_addr.sin_addr);
  server_addr.sin_port = htons(port);

  // Se le asigna al socket del servidor un puerto y una IP donde escuchar
  int ret2 = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

  // Se coloca el socket en modo listening
  int ret3 = listen(server_socket, 1);

  // Se definen las estructuras para almacenar info sobre los sockets de los clientes
  struct sockaddr_in client_addr;
  // struct sockaddr_in client1_addr;
  // struct sockaddr_in client2_addr;
  // struct sockaddr_in client3_addr;
  // struct sockaddr_in client4_addr;
  socklen_t addr_size = sizeof(client_addr);

  // Se inicializa una estructura propia para guardar los n°s de sockets de los clientes.
  PlayersInfo * sockets_clients = malloc(sizeof(PlayersInfo));

  // // Se aceptan a los primeros 2 clientes que lleguen. "accept" retorna el n° de otro socket asignado para la comunicación
  // printf("Esperando a que se conecte el primer cliente...\n");
  // sockets_clients->socket_c1 = accept(server_socket, (struct sockaddr *)&client1_addr, &addr_size);
  // printf("Primer cliente conectado!\n\n");

  // // Le enviamos al primer cliente un mensaje de bienvenida
  // char * welcome = "Bienvenido Cliente 1!!";
  // server_send_message(sockets_clients->socket_c1, 0, welcome);

  // printf("Esperando a que se conecte el segundo cliente...\n");
  // sockets_clients->socket_c2 = accept(server_socket, (struct sockaddr *)&client2_addr, &addr_size);
  // printf("Segundo cliente conectado!\n\n");

  // // Le enviamos al segundo cliente un mensaje de bienvenida
  // welcome = "Bienvenido Cliente 2!!";
  // server_send_message(sockets_clients->socket_c2, 0, welcome);

  int client_sockets[4] = {0};
  int num_clients = 0;

  printf("Waiting for clients to connect...\n");
  while (1) {
    if (num_clients < 4) {
      int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);

      for (int i = 0; i < 4; i++) {
        if (client_sockets[i] == 0) {
          // Use an empty slot to store the new client socket
          client_sockets[i] = client_socket;
          num_clients++;
          printf("Client %d connected!\n", i + 1);
          // printf("Client socket: %d\n", client_socket);

          // Send a welcome message to the new client
          char welcome_message[50];
          snprintf(welcome_message, sizeof(welcome_message), "Welcome, Client %d!", i + 1);
          server_send_message(client_socket, 0, welcome_message);

          // // Create a thread to handle this client
          // pthread_t client_thread;
          // ClientInfo client_info = {client_socket, i + 1, client_sockets, &num_clients};
          // pthread_create(&client_thread, NULL, client_handler, &client_info);

          break;
        }
      }
    } else {
      for (int i = 0; i < 4; i++) {
        if (client_sockets[i] != 0) {
          if (recv(client_sockets[i], NULL, 0, MSG_PEEK | MSG_DONTWAIT) == 0) {
            printf("Client %d disconnected!\n", i+1);
            close(client_sockets[i]);
            client_sockets[i] = 0;
            num_clients--;
          }
        }
      }
    }
  }

  // // Se aceptan hasta 4 clientes
  // printf("Waiting for clients to connect...\n");
  // for (int i = 0; 1 < 4; i++) {
  //   int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);

  //   if (num_clients < 4) {
  //     if (i == 0) {
  //       sockets_clients->socket_c1 = client_socket;
  //       printf("Client 1 connected!\n");
  //       char * welcome = "Welcome Client 1!";
  //       server_send_message(client_socket, 0, welcome);
  //     } else if (i == 1) {
  //       sockets_clients->socket_c2 = client_socket;
  //       printf("Client 2 connected!\n");
  //       char * welcome = "Welcome Client 2!";
  //       server_send_message(client_socket, 0, welcome);
  //     } else if (i == 2) {
  //       sockets_clients->socket_c3 = client_socket;
  //       printf("Client 3 connected!\n");
  //       char * welcome = "Welcome Client 3!";
  //       server_send_message(client_socket, 0, welcome);
  //     }
  //     else if (i == 3) {
  //       sockets_clients->socket_c4 = client_socket;
  //       printf("Client 4 connected!\n");
  //       char * welcome = "Welcome Client 4!";
  //       server_send_message(client_socket, 0, welcome);
  //     }
  //   } else {
  //     char * message = "Maximum number of connections reached. Try again later...";
  //     server_send_message(client_socket, 400, message);
  //     close(client_socket);
  //   }
  // }

  return sockets_clients;
};

ClientsInfo * connections_handler(char * IP, int port) {
  int server_socket, client_sockets[MAX_CLIENTS], num_clients = 0;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_len = sizeof(client_addr);

  // Create server socket
  // int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
  // if (server_socket == -1) {
    perror("Socket creation failed");
    exit(1);
  }

  // Configure server socket
  int opt = 1;
  // int ret = setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
  int ret;
  if ((ret = setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) == -1) {
    perror("Socket configuration failed");
    exit(1);
  }

  // Se guardan el puerto e IP en la estructura antes definida
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  inet_aton(IP, &server_addr.sin_addr);
  server_addr.sin_port = htons(port);

  // Bind the server socket
  if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
    perror("Binding failed");
    exit(1);
  }

  // Listen for incoming connections
  if (listen(server_socket, MAX_CLIENTS) == -1) {
    perror("Listening failed");
    exit(1);
  }

  // Initialize client sockets array
  for (int i = 0; i < MAX_CLIENTS; i++) {
    client_sockets[i] = -1;
  }

  ClientsInfo * clients_info = malloc(sizeof(ClientInfo));

  printf("Waiting for clients to connect...\n");
  while (1) {
    int new_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
    if (new_socket == -1) {
      perror("Accept failed");
    } else {
      if (num_clients < MAX_CLIENTS) {
        // Can accept more clients
        for (int i = 0; i < MAX_CLIENTS; i++) {
          if (client_sockets[i] == -1) {
            client_sockets[i] = new_socket;
            clients_info->client_sockets[i] = new_socket;
            num_clients++;

            // Create a thread to handle the new client
            pthread_t tid;
            pthread_create(&tid, NULL, client_handler, &client_sockets[i]);

            printf("Client %d connected!\n", i+1);
            break;
          }
        }
      } else {
        // Max clients reached, inform and close the new connection
        char * max_clients_msg = "Maximum number of connections reached. Try again later.\n";
        server_send_message(new_socket, 4, max_clients_msg);
        close(new_socket);
      }
    }
  }

  close(server_socket);
  return clients_info;
}

void * client_handler(void * arg) {
  int client_socket = *((int*)arg);

  // Send a welcome message and options to the client
  char * welcome_message = "¡Bienvenido a DCCorreo!";

  char * first_menu = 
  "\n*** DCCorreo ***\n"
  "----------------\n"
  "[1] Registrarse\n"
  "[2] Iniciar sesión\n"
  "[X] Salir\n"
  "\nIndique su opción (1, 2 o X): ";

  char * main_menu =
  "\n*** Bandeja de Entrada ***\n"
  "--------------------------\n"
  "[1] Enviar correo electrónico\n"
  "[2] Ver correos enviados y recibidos\n"
  "[3] Ver usuarios conectados y desconectados\n"
  "[X] Salir de la aplicación\n"
  "\nIndique su opción (1, 2, 3 o X): ";

  server_send_message(client_socket, 0, welcome_message);

  // Primera interacción
  while (1) {
    char buffer[1024];
    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received <= 0) {
      if (bytes_received == 0) {
        printf("Client in socket %d has disconnected\n", client_socket);
      } else {
        perror("Receive error");
      }
      close(client_socket);
      return NULL;
    } else {
      ;
    }
  }
}