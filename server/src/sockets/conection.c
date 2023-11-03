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

void connections_handler(char * IP, int port, ClientsInfo *clients, UsersInfo *users_info) {
  // ClientsInfo * clients_info = calloc(1, sizeof(ClientsInfo));
  int client_sockets[MAX_CLIENTS];
  // int server_socket, num_clients = 0;
  int server_socket;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_len = sizeof(client_addr);
  
  ClientsInfo *clients_info = clients;
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
}

void * client_handler(void * arg) {
  // int client_socket = *((int*)arg);
  Client * client = (Client *)arg;
  // UsersInfo * users_info = client->users_info;
  int client_socket = client->client_socket;
  int is_logged = 0;

  while (1) {
    if (!is_logged) {
      char *message = "Enviando a Menú de Inicio...\n";
      server_send_message(client_socket, 1, message);
      printf("Menú de Inicio\n");
    }
    else {
      char *message = "Enviando a Bandeja de Entrada...\n";
      server_send_message(client_socket, 2, message);
      printf("Bandeja de entrada\n");
    }

    int msg_code = server_receive_id(client_socket);
    printf("Message code: %d\n", msg_code);

    if (msg_code == 0) {
      // Exit App
      printf("Client in socket %d has disconnected\n", client_socket);
      close(client_socket);
      client->clients_info->client_sockets[client->client_number - 1] = -1;
      client->clients_info->num_clients--;
      if (is_logged) {
        client->user->is_logged = 0;
      }
      break;
    }

    else if (msg_code == 1) {
      // Register
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
    }

    else if (msg_code == 3) {
      // Send Message
      char * message = server_receive_payload(client_socket);
      printf("Received message: %s\n", message);

      // HACER REVISIONES DEL MENSAJE: TIPO, VALIDACIONES, CHECKEAR USUARIO, ARMAR EL STRUCT MESSAGE, ARMAR EL MESSAGE_LIST, AGREGAR EL NUEVO MENSAJE AL FINAL

      // int is_valid = 1;

      // char command[3], recipient[11], content[256];
      // unsigned int seconds;

      // if (sscanf(message, "%2[^,],%10[^,],%255[^,],%u", command, recipient, content, &seconds) == 4) {
      //   printf("[%s] Programando un mensaje en %us desde ahora...\n", command, seconds);
      //   if (strcmp(command, "pt") == 1 && strcmp(command, "pf") == 1) {
      //     is_valid = 0;
      //   }
      //   if (seconds < 1 || seconds > 255) {
      //     is_valid = 0;
      //   }
      // }
      // else if (sscanf(message, "%2[^,],%10[^,],%255[^,]", command, recipient, content) == 3) {
      //   printf("Enviando un mensaje inmediato...\n");
      //   if (strcmp(command, "it") == 1 && strcmp(command, "if") == 1) {
      //     is_valid = 0;
      //   }
      // }
      // else {
      //   printf("El formato del comando es inválido\n");
      //   is_valid = 0;
      // }

      // // verificar largo del contenido

      // int index_recipient = check_username(recipient, client->users_info) - 1;
      // if (index_recipient == -1) {
      //   is_valid = 0;
      // }

      // if (is_valid) {
      //   User *recipient = client->users_info->users_list[index_recipient];

      //   Message *new_message = malloc(sizeof(Message));
      //   new_message->sender = client->user->username;
      //   new_message->recipient = client->users_info->users_list[index_recipient]->username;
      //   *new_message->content = *content;

      //   if (strcmp(command, "it") == 0) {
      //     new_message->message_type = 0;
      //     new_message->is_downloaded = 1;
      //   }
      //   else if (strcmp(command, "if") == 0) {
      //     new_message->message_type = 1;
      //     new_message->is_downloaded = 0;
      //   }

      //   MessageList *new_sent = calloc(1, sizeof(MessageList));
      //   new_sent->message = new_message;

      //   MessageList *new_received = calloc(1, sizeof(MessageList));
      //   new_received->message = new_message;

      //   if (!client->user->sent_messages) {
      //     client->user->sent_messages = new_sent;
      //   } else {
      //     MessageList *current_message_list = client->user->sent_messages;
      //     while (1) {
      //       if (current_message_list->next) {
      //         current_message_list = current_message_list->next;
      //       }
      //       else {
      //         current_message_list->next = new_sent;
      //         break;
      //       }
      //     }
      //   }
        
      //   if (!recipient->sent_messages) {
      //     recipient->sent_messages = new_received;
      //   } else {
      //     MessageList *current_message_list = recipient->sent_messages;
      //     while (1) {
      //       if (current_message_list->next) {
      //         current_message_list = current_message_list->next;
      //       }
      //       else {
      //         current_message_list->next = new_received;
      //         break;
      //       }
      //     }
      //   }
        
      //   // if (!recipient->received_messages) {
      //   //   MessageList *received = calloc(1, sizeof(MessageList));
      //   //   recipient->received_messages = received;
      //   // }
        

      // }
      
      // printf("Mensajes enviados:\n");
      // MessageList *root = client->user->sent_messages;
      // while (1) {
      //   if (root->message) {
      //     printf("Mensaje enviado por %s a %s: %s\n", root->message->sender, root->message->recipient, root->message->content);
      //   } else {
      //     break;
      //   }

      //   if (root->next) {
      //     root = root->next;
      //   } else {
      //     break;
      //   }
      // }
    }
    
    // else if (msg_code == 4) {
    //   // Send Immediate File
    //   char * message = server_receive_payload(client_socket);
    //   printf("Received message: %s\n", message);
    // }

    // else if (msg_code == 5) {
    //   // Send Programmed Text
    //   char * message = server_receive_payload(client_socket);
    //   printf("Received message: %s\n", message);
    // }

    // else if (msg_code == 6) {
    //   // Send Programmed File
    //   char * message = server_receive_payload(client_socket);
    //   printf("Received message: %s\n", message);
    // }

    else if (msg_code == 7) {
      // Correos enviados y recibidos
      ;
    }

    else if (msg_code == 8) {
      // Usuarios conectados y desconectados
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
    }

    else {
      // Not valid code
      char * error_msg = "Opción no válida.\n";
      server_send_message(client_socket, 0, error_msg);
    }
  }

  return NULL;
}

// bool check_username(char user, char * users_list[6]) {
//   // Función que revisa si el usuario se encuentra registrado o no, retorna 1 si está en la lista y 0 si no
//   ;
// }