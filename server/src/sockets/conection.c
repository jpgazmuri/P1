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

int check_username(char username, UsersInfo *users_info) {
  for (int i = 0; i < users_info->num_users; i++) {
    if (username == *users_info->users_list[i]->username) {
      return i + 1;
    }
  }

  return 0;
}

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

ClientsInfo * connections_handler(char * IP, int port, UsersInfo *users_info) {
  // ClientsInfo * clients_info = calloc(1, sizeof(ClientsInfo));
  int client_sockets[MAX_CLIENTS];
  // int server_socket, num_clients = 0;
  int server_socket;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_len = sizeof(client_addr);
  
  ClientsInfo * clients_info = malloc(sizeof(ClientInfo));
  clients_info->num_clients = 0;
  // UsersInfo * users_info = calloc(1, sizeof(UsersInfo));
  // users_info->num_users = 0;

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
    clients_info->client_sockets[i] = -1;
  }

  printf("Waiting for clients to connect...\n");
  while (1) {
    int new_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
    if (new_socket == -1) {
      perror("Accept failed");
    } else {
      if (clients_info->num_clients < MAX_CLIENTS) {
        // Can accept more clients
        for (int i = 0; i < MAX_CLIENTS; i++) {
          if (clients_info->client_sockets[i] == -1) {
            client_sockets[i] = new_socket;
            clients_info->client_sockets[i] = new_socket;
            clients_info->num_clients++;

            // Create a thread to handle the new client
            // Client client = {new_socket, i + 1, clients_info};
            Client client = {new_socket, i + 1, clients_info, users_info, NULL};
            pthread_t tid;
            pthread_create(&tid, NULL, client_handler, &client);
            // pthread_create(&tid, NULL, client_handler, &client_sockets[i]);

            printf("Client %d connected to socket %d!\n", i+1, new_socket);
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
  // int client_socket = *((int*)arg);
  Client * client = (Client *)arg;
  // UsersInfo * users_info = client->users_info;
  int client_socket = client->client_socket;
  int is_logged = 0;

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

  // Menú de inicio
  while (!is_logged) {
    server_send_message(client_socket, 1, first_menu);
    // Mientras el cliente no haya iniciado sesión, se muestra el menú de inicio
    printf("Menú de inicio\n");

    int msg_code = server_receive_id(client_socket);
    printf("Message code: %d\n", msg_code);
    if (msg_code == 0) {
      // Salió de la app
      printf("Client in socket %d has disconnected\n", client_socket);
      close(client_socket);
      client->clients_info->client_sockets[client->client_number - 1] = -1;
      client->clients_info->num_clients--;
      break;
    }
    else if (msg_code == 1) {
      // Registro
      char * username = server_receive_payload(client_socket);
      printf("Username for register: %s\n", username);

      if (strlen(username) < 1 || strlen(username) > 10) {
        printf("Username doesn't satisfy format\n");
        char * error_msg = "El nombre de usuario no es válido, debe ser entre 1 y 10 carácteres.";
        server_send_message(client_socket, 0, error_msg);
      }
      else {
        if (client->users_info->num_users >= 6) {
          // Límite de registros alcanzado
          char * error_msg = "El límite de usuarios registrados ha sido alcanzado\n";
          server_send_message(client_socket, 0, error_msg);
        }
        else if (check_username(*username, client->users_info)) {
          char * error_msg = "El nombre de usuario ya está registrado\n";
          server_send_message(client_socket, 0, error_msg);
        }
        else {
          User *new_user = calloc(1, sizeof(User));
          new_user->username = username;
          new_user->is_logged = 1;
          printf("New user %s with logged in status %d\n", new_user->username, new_user->is_logged);
          client->users_info->users_list[client->users_info->num_users] = new_user;
          client->users_info->num_users++;
          client->user = new_user;
          is_logged = 1;
        }
      }
    } 
    else if (msg_code == 2) {
      // Login
      char * username = server_receive_payload(client_socket);
      printf("Username for login: %s\n", username);

      if (strlen(username) < 1 || strlen(username) > 10) {
        printf("Username doesn't satisfy format\n");
        char * error_msg = "El nombre de usuario no es válido, debe ser entre 1 y 10 carácteres.";
        server_send_message(client_socket, 0, error_msg);
      }
      else {
        if (!check_username(*username, client->users_info)) {
          char * error_msg = "El nombre de usuario no existe\n";
          server_send_message(client_socket, 0, error_msg);
        }
        else {
          int user_index = check_username(*username, client->users_info) - 1;
          if (client->users_info->users_list[user_index]->is_logged) {
            printf("User #%d with username %s is already logged in\n", user_index + 1, client->users_info->users_list[user_index]->username);
            char *error_msg = "Este usuario está actualmente conectado, intente más tarde\n";
            server_send_message(client_socket, 0, error_msg);
          }
          else {
            client->user = client->users_info->users_list[user_index];
            client->user->is_logged = 1;
            printf("Client %s logged in to user #%d with username %s\n", username, user_index + 1, client->user->username);
            is_logged = 1;
          }
        }
      }
    } else {
      char * error_msg = "Esa no es una opción válida, intente nuevamente.\n";
      server_send_message(client_socket, 0, error_msg);
    }
  }

  printf("\nLista de usuarios:\n");
  for (int i = 0; i < client->users_info->num_users; i++) {
    printf("User #%d: %s - logged? %d\n", i + 1, client->users_info->users_list[i]->username, client->users_info->users_list[i]->is_logged);
  }
  printf("\n");

  // Menú principal
  while (is_logged) {
    server_send_message(client_socket, 2, main_menu);
    printf("Menú principal\n");
    
    int msg_code = server_receive_id(client_socket);
    printf("Message code: %d\n", msg_code);
    if (msg_code == 0) {
      // Salir app
      printf("Client in socket %d has disconnected\n", client_socket);
      close(client_socket);
      client->clients_info->client_sockets[client->client_number - 1] = -1;
      client->clients_info->num_clients--;
      client->user->is_logged = 0;
      break;
    } else if (msg_code == 1) {
      // Enviar correo electrónico
      ;
    } else if (msg_code == 2) {
      // Ver correos enviados y recibidos
      ;
    } else if (msg_code == 3) {
      // Ver usuarios conectados y desconectados
      char *response = malloc(512);
      char *connected_users = malloc(512);
      char *disconnected_users = malloc(512);

      strcpy(response, "\n*** Usuarios Conectados ***\n");
      strcat(response, "---------------------------\n");
      int n_connected = 0, n_disconnected = 0;
      for (int i = 0; i < client->users_info->num_users; i++) {
        User *current_user = client->users_info->users_list[i];
        char entry[512];
        if (current_user->is_logged) {
          snprintf(entry, sizeof(entry), "[%d] %s\n", n_connected + 1, client->users_info->users_list[i]->username);
          n_connected++;
          strcat(connected_users, entry);
        }
        else {
          snprintf(entry, sizeof(entry), "[%d] %s\n", n_disconnected + 1, client->users_info->users_list[i]->username);
          n_disconnected++;
          strcat(disconnected_users, entry);
        }
        // snprintf(entry, sizeof(entry), "[%d] %s\n", i + 1, client->users_info->users_list[i]->username);
        // strcat(response, entry);
        // strcat(response, "[");
        // strcat(response, i + 1);
        // strcat(response, "] ");
        // strcat(response, client->users_info->users_list[i]);
        // strcat(response, "\n");
      }

      if (n_connected > 0) {
        strcat(response, connected_users);
      }
      else {
        strcat(response, "No hay usuarios conectados\n");
      }
      // strcat(response, "\n");
      strcat(response, "\n*** Usuarios Desconectados ***\n");
      strcat(response, "------------------------------\n");
      if (n_disconnected > 0) {
        strcat(response, disconnected_users);
      }
      else {
        strcat(response, "No hay usuarios desconectados\n");
      }
      strcat(response, "\n");
      server_send_message(client_socket, 0, response);
      free(response);
      free(connected_users);
      free(disconnected_users);
    } else {
      char * error_msg = "Opción no válida, intente nuevamente.\n";
      server_send_message(client_socket, 0, error_msg);
    }
  }
  return NULL;
}

// bool check_username(char user, char * users_list[6]) {
//   // Función que revisa si el usuario se encuentra registrado o no, retorna 1 si está en la lista y 0 si no
//   ;
// }