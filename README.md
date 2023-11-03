# P1
Repositorio para la entrega P1 de Sistemas Operativos y Redes de los alumnos Jose Pedro Gazmuri y Rosario González.

## IDs de mensajes del server
A continuación se describe la lógica detrás de la asignación de IDs para los mensajes enviados por el servidor al cliente:
0. *Unrepliable message:* mensajes del servidor que no requieren respuesta del usuario, principalmente para mostrarle info solicitada, dar avisos, comunicar errores, etc.
1. *Menú inical:* indica al cliente que se encuentra en el menú inicial (registro/inicio de sesión).
2. *Menú principal:*
3. *Solicitar archivo:* solicita al cliente un archivo tras enviar un mensaje archivo.
4. *Descargar archivo:* indica al cliente descargar un archivo que ha recibido por correo.

## IDs de mensajes del cliente
A continuación se describe la lógica detrás de la asignación de IDs para los mensajes enviados por el cliente al servidor:
0. *Exit App:* significa que el cliente cierra la aplicación y se debe cerrar el socket.
1. *Register:* el usuario envía una solicitud de registro, con su nombre de usuario en el payload del mensaje.
2. *Login:* el usuario solicita iniciar sesión con el nombre de usuario almacenado en el payload.
3. *Send Message:* el usuario solicita enviar un correo a otro usuario.
7. *Correos Enviados y Recibidos:* el usuario solicita al servidor la lista de correos enviados y recibidos.
8. *Usuarios Conectados y Desconectados:* el usuario solicita al servidor el listado de usuarios conectados y desconectados.
9. *Client side error:* anuncia al servidor que hubo un error del lado del cliente.