// server.c

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <windows.h>  // Para ejecutar comandos nativos de Windows

#pragma comment(lib, "Ws2_32.lib")

#define BUFFER_SIZE 1024

int main(int argc, char const* argv[]) 
{
    WSADATA wsaData;

    // Inicializa la librería Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Error al inicializar Winsock: %d\n", WSAGetLastError());
        return 1;
    }

    // Crea un socket para el servidor
    SOCKET servSockD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (servSockD == INVALID_SOCKET) {
        printf("Error al crear el socket: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Configura la dirección del servidor
    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(9001);  // Puerto del servidor
    servAddr.sin_addr.s_addr = INADDR_ANY; // Escucha en todas las interfaces locales

    // Asocia el socket a la dirección y puerto configurados
    if (bind(servSockD, (struct sockaddr*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        printf("Error al hacer bind: %d\n", WSAGetLastError());
        closesocket(servSockD);
        WSACleanup();
        return 1;
    }

    // Comienza a escuchar conexiones entrantes
    if (listen(servSockD, 1) == SOCKET_ERROR) {
        printf("Error al escuchar: %d\n", WSAGetLastError());
        closesocket(servSockD);
        WSACleanup();
        return 1;
    }

    printf("Esperando conexion de un cliente...\n");

    // Acepta una conexión entrante
    SOCKET clientSocket = accept(servSockD, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        printf("Error al aceptar conexion: %d\n", WSAGetLastError());
        closesocket(servSockD);
        WSACleanup();
        return 1;
    }

    printf("Cliente conectado.\n");

    char buffer[BUFFER_SIZE];
    char result[BUFFER_SIZE];

    while (1) {
        // Recibe datos del cliente
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0'; // Asegura que el buffer sea una cadena válida

            // Si el cliente envía "salida", cierra la conexión
            if (strcmp(buffer, "salida") == 0) {
                printf("Cliente desconectado.\n");
                break;
            }

            // Limpia el contenido previo de 'result' antes de procesar el nuevo comando
            memset(result, 0, BUFFER_SIZE);

            printf("Comando recibido: %s\n", buffer);

            // Procesa comandos específicos
            if (strncmp(buffer, "cd", 2) == 0) {
                char* dir = buffer + 3; // Obtiene el directorio deseado
                if (_chdir(dir) == 0) {
                    snprintf(result, BUFFER_SIZE, "Cambio de directorio exitoso.\n");
                } else {
                    snprintf(result, BUFFER_SIZE, "Error al cambiar de directorio.\n");
                }
            } else if (strcmp(buffer, "ls") == 0 || strcmp(buffer, "dir") == 0) {
                // Lista archivos en el directorio actual
                FILE* fp = popen("dir", "r");
                if (fp == NULL) {
                    snprintf(result, BUFFER_SIZE, "Error al ejecutar el comando.\n");
                } else {
                    fread(result, 1, BUFFER_SIZE - 1, fp);
                    pclose(fp);
                }
            } else if (strcmp(buffer, "pwd") == 0) {
                // Devuelve el directorio actual
                char cwd[BUFFER_SIZE];
                if (_getcwd(cwd, sizeof(cwd)) != NULL) {
                    snprintf(result, BUFFER_SIZE, "Directorio actual: %s\n", cwd);
                } else {
                    snprintf(result, BUFFER_SIZE, "Error al obtener el directorio actual.\n");
                }
            } else if (strcmp(buffer, "clear") == 0) {
                // Comando para limpiar pantalla
                snprintf(result, BUFFER_SIZE, "clear");
            } else if (strncmp(buffer, "echo", 4) == 0) {
                // Devuelve el texto proporcionado
                char* message = buffer + 5;
                snprintf(result, BUFFER_SIZE, "%s\n", message);
            } else if (strcmp(buffer, "time") == 0) {
                // Devuelve la hora actual del sistema
                FILE* fp = popen("time /t", "r");
                if (fp == NULL) {
                    snprintf(result, BUFFER_SIZE, "Error al ejecutar el comando.\n");
                } else {
                    fread(result, 1, BUFFER_SIZE - 1, fp);
                    pclose(fp);
                }
            } else if (strcmp(buffer, "hostname") == 0) {
                // Devuelve el nombre del host
                FILE* fp = popen("hostname", "r");
                if (fp == NULL) {
                    snprintf(result, BUFFER_SIZE, "Error al obtener el nombre del host.\n");
                } else {
                    fread(result, 1, BUFFER_SIZE - 1, fp);
                    pclose(fp);
                }
            } else {
                // Comando no reconocido
                snprintf(result, BUFFER_SIZE, "Comando no reconocido.\n");
            }

            // Envía la respuesta al cliente
            send(clientSocket, result, strlen(result), 0);
        } else {
            printf("Error al recibir datos del cliente: %d\n", WSAGetLastError());
            break;
        }
    }

    // Cierra los sockets y limpia Winsock
    closesocket(clientSocket);
    closesocket(servSockD);
    WSACleanup();

    return 0;
}