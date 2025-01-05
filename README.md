# Aplicación Cliente/Servidor.

## Descripción
Este proyecto implementa una aplicación cliente/servidor en C que permite la ejecución remota de comandos. El cliente se conecta al servidor a través de sockets TCP, envía comandos que se ejecutan en el servidor, y recibe las respuestas para mostrarlas localmente. Es compatible con sistemas Windows utilizando la biblioteca Winsock.

---

## Características Principales
1. **Comunicación Cliente/Servidor:**
   - Utiliza sockets TCP para enviar y recibir datos.
   - Manejo robusto de errores en la conexión y transferencia de datos.

2. **Soporte de Comandos:**
   - `cd <directorio>`: Cambia el directorio actual en el servidor.
   - `ls` o `dir`: Lista los archivos en el directorio actual.
   - `pwd`: Muestra el directorio actual.
   - `clear`: Limpia la pantalla del cliente.
   - `echo <texto>`: Devuelve el texto especificado al cliente.
   - `time`: Muestra la hora actual.
   - `hostname`: Muestra el nombre del host.
   - Manejo de comandos no reconocidos con mensajes de error.

3. **Compatibilidad con Windows:**
   - Uso de funciones de la biblioteca Winsock („.g., `WSAStartup`, `WSACleanup`).
   - Adaptación de comandos para el entorno Windows.

---

## Requisitos del Sistema
- Windows con soporte para Winsock 2.
- Compilador compatible con C (e.g., GCC o Visual Studio).

---

## Configuración y Ejecución
### Compilación
1. Compila el servidor:
   ```bash
   gcc server.c -o server -lws2_32
   ```

2. Compila el cliente:
   ```bash
   gcc client.c -o client -lws2_32
   ```

### Ejecución
1. Inicia el servidor:
   ```bash
   ./server
   ```

2. Inicia el cliente en una terminal diferente:
   ```bash
   ./client
   ```

3. Introduce comandos en el cliente para interactuar con el servidor.

### Finalización
- Para terminar la conexión, escriba `salida` en el cliente.

---

## Estructura del Proyecto
- **`server.c`**: Contiene el código del servidor que ejecuta comandos recibidos del cliente.
- **`client.c`**: Contiene el código del cliente que envía comandos al servidor y muestra las respuestas.

---

## Problemas Comunes
1. **Error al inicializar Winsock:**
   - Asegúrate de que Winsock esté correctamente configurado en tu sistema.

2. **Conexión rechazada:**
   - Verifique que el servidor esté en ejecución y que la dirección IP y el puerto sean correctos.

3. **Comandos no ejecutados:**
   - Asegúrese de que los comandos enviados sean compatibles con el entorno Windows.

---

## Autor
DeibyMP  
Contacto: deibymuoz2002@gmail.com

