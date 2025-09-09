#ifndef SOCKET_STUFF_H
#define SOCKET_STUFF_H

#include <iostream>
#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
typedef int SOCKET;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif


extern SOCKET socket_global;

SOCKET setup_socket();

void close_socket(SOCKET sock);

bool broadcast_message(SOCKET sock, const std::string& msg, const std::string& address, int port);

#endif