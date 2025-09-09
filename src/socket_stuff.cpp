// udp socket utils

#include <iostream>
#include <cstring>

#include "socket_stuff.h"

SOCKET socket_global;

SOCKET setup_socket() {
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Failed to create socket." << std::endl;
        return INVALID_SOCKET;
    }

    int enableBroadcast = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&enableBroadcast, sizeof(enableBroadcast)) == SOCKET_ERROR) {
        std::cerr << "Failed to enable broadcast." << std::endl;
#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
        return INVALID_SOCKET;
    }

    return sock;
}

void close_socket(SOCKET sock) {
#ifdef _WIN32
    closesocket(sock);
#else
    close(sock);
#endif
}

bool broadcast_message(SOCKET sock, const std::string& msg, const std::string& address, int port) {
    sockaddr_in serverAddress;
    std::memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = inet_addr(address.c_str());

    if (sendto(sock, msg.c_str(), msg.length(), 0, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Failed to send broadcast message." << std::endl;
        return false;
    }

    return true;
}