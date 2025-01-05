// client

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "Ws2_32.lib")

#define BUFFER_SIZE 1024

int main(int argc, char const* argv[]) 
{
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Error al inicializar Winsock: %d\n", WSAGetLastError());
        return 1;
    }

    SOCKET sockD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockD == INVALID_SOCKET) {
        printf("Error al crear el socket: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(9001);
    if (inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr) <= 0) {
        printf("Error al convertir la dirección IP.\n");
        closesocket(sockD);
        WSACleanup();
        return 1;
    }

    if (connect(sockD, (struct sockaddr*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        printf("Error al conectar al servidor: %d\n", WSAGetLastError());
        closesocket(sockD);
        WSACleanup();
        return 1;
    }

    printf("Conexion establecida con el servidor.\nEscribe comandos para ejecutar o 'salida' para terminar.\n");

    char command[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    while (1) {
        printf("$ ");
        fgets(command, BUFFER_SIZE, stdin);
        command[strcspn(command, "\n")] = 0; // Eliminar el salto de línea

        if (strcmp(command, "salida") == 0) {
            send(sockD, command, strlen(command), 0);
            break;
        }

        send(sockD, command, strlen(command), 0);

        int bytesReceived = recv(sockD, response, BUFFER_SIZE - 1, 0);
        if (bytesReceived > 0) {
            response[bytesReceived] = '\0';

            // Si el servidor envía "clear", limpia la pantalla del cliente
            if (strcmp(response, "clear") == 0) {
                system("cls");
            } else if (strcmp(response, "Comando no reconocido.") == 0) {
                printf("Error: %s\n", response);
            } else {
                // Imprimir la respuesta si el comando fue reconocido
                printf("%s\n", response);
            }
        } else {
            printf("Error al recibir datos del servidor: %d\n", WSAGetLastError());
        }
    }

    closesocket(sockD);
    WSACleanup();

    return 0;
}