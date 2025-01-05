// server

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

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Error al inicializar Winsock: %d\n", WSAGetLastError());
        return 1;
    }

    SOCKET servSockD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (servSockD == INVALID_SOCKET) {
        printf("Error al crear el socket: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(9001);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(servSockD, (struct sockaddr*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        printf("Error al hacer bind: %d\n", WSAGetLastError());
        closesocket(servSockD);
        WSACleanup();
        return 1;
    }

    if (listen(servSockD, 1) == SOCKET_ERROR) {
        printf("Error al escuchar: %d\n", WSAGetLastError());
        closesocket(servSockD);
        WSACleanup();
        return 1;
    }

    printf("Esperando conexion de un cliente...\n");

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
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';

            if (strcmp(buffer, "salida") == 0) {
                printf("Cliente desconectado.\n");
                break;
            }

            // Limpiar el contenido previo de 'result' antes de procesar el nuevo comando
            memset(result, 0, BUFFER_SIZE);

            printf("Comando recibido: %s\n", buffer);

            // Manejo del comando 'cd'
            if (strncmp(buffer, "cd", 2) == 0) {
                char* dir = buffer + 3;
                if (_chdir(dir) == 0) {
                    snprintf(result, BUFFER_SIZE, "Cambio de directorio exitoso.\n");
                } else {
                    snprintf(result, BUFFER_SIZE, "Error al cambiar de directorio.\n");
                }
            } 
            // Manejo del comando 'ls' o 'dir'
            else if (strcmp(buffer, "ls") == 0 || strcmp(buffer, "dir") == 0) {
                FILE* fp = popen("dir", "r");
                if (fp == NULL) {
                    snprintf(result, BUFFER_SIZE, "Error al ejecutar el comando.\n");
                } else {
                    fread(result, 1, BUFFER_SIZE - 1, fp);
                    pclose(fp);
                }
            } 
            // Manejo del comando 'pwd'
            else if (strcmp(buffer, "pwd") == 0) {
                char cwd[BUFFER_SIZE];
                if (_getcwd(cwd, sizeof(cwd)) != NULL) {
                    snprintf(result, BUFFER_SIZE, "Directorio actual: %s\n", cwd);
                } else {
                    snprintf(result, BUFFER_SIZE, "Error al obtener el directorio actual.\n");
                }
            }
            // Manejo del comando 'clear'
            else if (strcmp(buffer, "clear") == 0) {
                snprintf(result, BUFFER_SIZE, "clear");
            }
            // Manejo del comando 'echo'
            else if (strncmp(buffer, "echo", 4) == 0) {
                char* message = buffer + 5;
                snprintf(result, BUFFER_SIZE, "%s\n", message);
            }
            // Manejo del comando 'time'
            else if (strcmp(buffer, "time") == 0) {
                FILE* fp = popen("time /t", "r");
                if (fp == NULL) {
                    snprintf(result, BUFFER_SIZE, "Error al ejecutar el comando.\n");
                } else {
                    fread(result, 1, BUFFER_SIZE - 1, fp);
                    pclose(fp);
                }
            }

            // Comando 'hostname' para obtener el nombre del host
            else if (strcmp(buffer, "hostname") == 0) {
                FILE* fp = popen("hostname", "r");
                if (fp == NULL) {
                    snprintf(result, BUFFER_SIZE, "Error al obtener el nombre del host.\n");
                } else {
                    fread(result, 1, BUFFER_SIZE - 1, fp);
                    pclose(fp);
                }
            }
            // Si el comando no es reconocido, envia un mensaje de error
            else {
                snprintf(result, BUFFER_SIZE, "Comando no reconocido.\n");
            }

            send(clientSocket, result, strlen(result), 0);
        } else {
            printf("Error al recibir datos del cliente: %d\n", WSAGetLastError());
            break;
        }
    }

    closesocket(clientSocket);
    closesocket(servSockD);
    WSACleanup();

    return 0;
}