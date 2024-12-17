#include <iostream>
#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif
#include "simulator.h"

int server_socket = -1, client_socket = -1;
const int port = 12345;
char buffer[512];

//Setup socket for host
void hostSocket() {
    #ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "[ERROR] Winsock initialization failed.\n";
        std::exit(-1);
    }
    #endif

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        std::cerr << "[ERROR] Socket creation failed.\n" ;
        std::exit(-1);
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "[ERROR] Binding failed.\n";
        std::exit(-1);
    }

    if (listen(server_socket, 5) < 0) {
        std::cerr << "[ERROR] Listening failed.\n";
        std::exit(-1);
    }
    std::cout << "[INFO] Server listening on port " << port << ".\n";

    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    client_socket = accept(server_socket, (sockaddr*)&client_addr, &client_len);
    if (client_socket < 0) {
        std::cerr << "[ERROR] Connection acceptance failed.\n";
        std::exit(-1);
    }
    std::cout << "[INFO] Client has connected.\n";
}

//Connect to host with given ip
void joinSocket() {
    #ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "[ERROR] Winsock initialization failed.\n";
        std::exit(-1);
    }
    #endif

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    while (true) {
        std::string server_ip;
        std::cout << "Enter server IP address: ";
        std::cin >> server_ip;
        //server_ip = "127.0.0.1";
        if (inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr) > 0) {
            break;
        } else {
            std::cerr << "[ERROR] Invalid IP address. Please try again.\n";
        }
    }

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        std::cerr << "[ERROR] Socket creation failed.\n";
        std::exit(-1);
    }

    if (connect(client_socket, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "[ERROR] Connection to the server failed.\n";
        std::exit(-1);
    }
    std::cout << "[INFO] Connected to the server.\n";
}

//Send game state to other player
void sendGameState() {
    int offset = 0;
    for (int i = 0; i < 16; ++i) {
        offset += std::sprintf(buffer + offset, "%f,%f%s", balls[i].x, balls[i].y, (i < 15 ? "," : ""));
    }
    offset += std::sprintf(buffer + offset, ",%f,%f", balls[15].i, balls[15].j);
    int bytes_sent = send(client_socket, buffer, strlen(buffer), 0);
    if (bytes_sent < 0) {
        std::cerr << (gameMode == 0 ? "[ERROR] Failed to send game state to client.\n" : "[ERROR] Failed to send game state to server.\n");
        return;
    }
}

//Recieve and save game state
void receiveGameState() {
    while (running) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            //std::cout << (gameMode == 0 ? "Client: " : "Server: ") << buffer << "\n";
            const char* ptr = buffer;
            for (int i = 0; i < 16; ++i) {
                std::sscanf(ptr, "%f,%f", &balls[i].x, &balls[i].y);
                ptr = std::strchr(ptr, ',') + 1;
                ptr = std::strchr(ptr, ',') + 1;
            }
            std::sscanf(ptr, "%f,%f", &balls[15].i, &balls[15].j);
            balls[ball_n - 1].moving = true;
            active = true;
        } else if (bytes_received == 0) {
            std::cerr << (gameMode == 0 ? "[INFO] Connection closed by client.\n" : "[INFO] Connection closed by server.\n");
            running = false;
        } else {
            std::cerr << "[ERROR] Connection lost.\n";
            running = false;
        }
    }
}

//Termiante socket
void closeSocket() {
    if (client_socket >= 0) {
        #ifdef _WIN32
        shutdown(client_socket, SD_BOTH);
        closesocket(client_socket);
        #else
        shutdown(client_socket, SHUT_RDWR);
        close(client_socket);
        #endif
    }
    if (server_socket >= 0) {
        #ifdef _WIN32
        shutdown(server_socket, SD_BOTH);
        closesocket(server_socket);
        #else
        shutdown(server_socket, SHUT_RDWR);
        close(server_socket);
        #endif
    }
    #ifdef _WIN32
    WSACleanup();
    #endif
    running = false;
}