#include "conection.h"

int connected_clients = 0; 

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
  struct sockaddr_in client1_addr;
  struct sockaddr_in client2_addr;
  socklen_t addr_size = sizeof(client1_addr);

  // Se inicializa una estructura propia para guardar los n°s de sockets de los clientes.
  PlayersInfo * sockets_clients = malloc(sizeof(PlayersInfo));

  // Se aceptan a los primeros 2 clientes que lleguen. "accept" retorna el n° de otro socket asignado para la comunicación
    printf("Esperando a que se conecte el primer cliente...\n");
    int new_client_socket = accept(server_socket, (struct sockaddr *)&client1_addr, &addr_size);

    if (connected_clients >= 4) {
        printf("Se alcanzó el límite de clientes conectados. Rechazando nueva conexión.\n");
        // Aquí puedes notificar al cliente que se ha alcanzado el límite y cerrar la conexión.
        // También puedes agregar lógica adicional según tus requisitos.
        // Por ejemplo:
        // send(new_client_socket, "Límite de clientes alcanzado. Intente más tarde.\n", strlen("Límite de clientes alcanzado. Intente más tarde.\n"), 0);
        close(new_client_socket);  // Cierra la conexión del cliente
    } else {
    // Si no se ha alcanzado el límite de clientes, continúa con el proceso normalmente.
    // Agrega el nuevo socket del cliente a tu estructura de datos y realiza otras configuraciones necesarias.
    sockets_clients->socket_c1 = new_client_socket;

    // Incrementa el contador de clientes conectados
    connected_clients++;

    // Le enviamos al primer cliente un mensaje de bienvenida
    char * welcome = "Bienvenido Cliente 1!!";
    server_send_message(sockets_clients->socket_c1, 0, welcome);

    printf("Primer cliente conectado! (Clientes conectados: %d)\n\n", connected_clients);
}

  // Le enviamos al primer cliente un mensaje de bienvenida
  char * welcome = "Bienvenido Cliente 1!!";
  server_send_message(sockets_clients->socket_c1, 0, welcome);

  printf("Esperando a que se conecte el segundo cliente...\n");
  int new_client_socket2 = accept(server_socket, (struct sockaddr *)&client2_addr, &addr_size);

  if (connected_clients >= 4) {
      printf("Se alcanzó el límite de clientes conectados. Rechazando nueva conexión.\n");
      // Notificar y cerrar la conexión del cliente
      close(new_client_socket2);
  } else {
    // Continuar con el proceso normalmente
    sockets_clients->socket_c2 = new_client_socket2;
    connected_clients++;

    char * welcome2 = "Bienvenido Cliente 2!!";
    server_send_message(sockets_clients->socket_c2, 0, welcome2);

    printf("Segundo cliente conectado! (Clientes conectados: %d)\n\n", connected_clients);
  }


  // Le enviamos al segundo cliente un mensaje de bienvenida
  welcome = "Bienvenido Cliente 2!!";
  server_send_message(sockets_clients->socket_c2, 0, welcome);

  return sockets_clients;
}
