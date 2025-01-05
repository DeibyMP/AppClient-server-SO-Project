// client.c

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Vinculamos la librería de Winsock
#pragma comment(lib, "Ws2_32.lib")

#define BUFFER_SIZE 1024 // Tamaño del búfer para enviar y recibir datos

int main(int argc, char const* argv[]) 
{
    WSADATA wsaData;

    // Inicializamos Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Error al inicializar Winsock: %d\n", WSAGetLastError());
        return 1;
    }

    // Creamos un socket
    SOCKET sockD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockD == INVALID_SOCKET) {
        printf("Error al crear el socket: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Configuramos la dirección del servidor
    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET; // Familia de direcciones IPv4
    servAddr.sin_port = htons(9001); // Puerto en el que se conecta el servidor
    if (inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr) <= 0) { // Dirección IP del servidor
        printf("Error al convertir la dirección IP.\n");
        closesocket(sockD);
        WSACleanup();
        return 1;
    }

    // Intentamos conectarnos al servidor
    if (connect(sockD, (struct sockaddr*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        printf("Error al conectar al servidor: %d\n", WSAGetLastError());
        closesocket(sockD);
        WSACleanup();
        return 1;
    }

    printf("Conexion establecida con el servidor.\nEscribe comandos para ejecutar o 'salida' para terminar.\n");

    char command[BUFFER_SIZE]; // Almacena comandos ingresados por el usuario
    char response[BUFFER_SIZE]; // Almacena respuestas del servidor

    while (1) {
        // Solicitamos un comando al usuario
        printf("$ ");
        fgets(command, BUFFER_SIZE, stdin); // Leemos el comando del usuario
        command[strcspn(command, "\n")] = 0; // Eliminamos el salto de línea al final

        // Si el usuario escribe 'salida', cerramos la conexión
        if (strcmp(command, "salida") == 0) {
            send(sockD, command, strlen(command), 0); // Enviamos el comando al servidor
            break;
        }

        // Enviamos el comando al servidor
        send(sockD, command, strlen(command), 0);

        // Recibimos la respuesta del servidor
        int bytesReceived = recv(sockD, response, BUFFER_SIZE - 1, 0);
        if (bytesReceived > 0) {
            response[bytesReceived] = '\0'; // Aseguramos que la respuesta esté terminada en null

            // Si el servidor envía "clear", limpiamos la pantalla del cliente
            if (strcmp(response, "clear") == 0) {
                system("cls"); // Comando para limpiar la consola en Windows
            } 
            // Si el servidor no reconoce el comando, mostramos un mensaje de error
            else if (strcmp(response, "Comando no reconocido.") == 0) {
                printf("Error: %s\n", response);
            } 
            // Para comandos reconocidos, mostramos la respuesta del servidor
            else {
                printf("%s\n", response);
            }
        } else {
            // Si ocurre un error al recibir datos, mostramos el código del error
            printf("Error al recibir datos del servidor: %d\n", WSAGetLastError());
        }
    }

    // Cerramos el socket y limpiamos Winsock
    closesocket(sockD);
    WSACleanup();

    return 0;
}