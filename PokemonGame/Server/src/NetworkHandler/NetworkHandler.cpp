// NetworkHandler.cpp
#include "NetworkHandler.h"
#include <iostream>
#include <string>
using namespace std;

#define MAX_SIZE 1024

NetworkHandler::NetworkHandler(SOCKET &socket) : socket_(socket) {
}

NetworkHandler::~NetworkHandler() {
    closesocket(socket_);
}


int NetworkHandler::peek(std::string ip)
{
    char buffer[1];
    int result = recv(socket_, buffer, 1, MSG_PEEK);

    if (result == 0) {
        std::cerr << ip << " closed unexpectedly." << std::endl;
        return SOCKET_ERROR;
    }
    else if (result == SOCKET_ERROR) {
        if (WSAGetLastError() == WSAECONNRESET) {
            std::cerr << ip << " closed unexpectedly." << std::endl;
        }
        else {
            std::cerr << "Recv failed with error: " << WSAGetLastError() << std::endl;
        }
        return SOCKET_ERROR;
    }
    return 200;
}

int NetworkHandler::sendData(const char* data) {
    
        int bytesSent = send(socket_, data, strlen(data), 0);
        //cout << data[3] << endl;
        if (bytesSent == SOCKET_ERROR) {
            std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
            //closesocket(socket_);
            WSACleanup();
            return SOCKET_ERROR;
        }
        return bytesSent;
    
}

int NetworkHandler::sendData(int code, std::string msg) {
    
        std::string message = std::to_string(code) + " " + msg;

        int bytesSent = send(socket_, message.c_str(), message.size(), 0);
        if (bytesSent == SOCKET_ERROR) {
            std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
            //closesocket(socket_);
            WSACleanup();
            return SOCKET_ERROR;
        }
        return bytesSent;
    
}


int NetworkHandler::receiveData(char* buffer) {
    int bytesReceived = recv(socket_, buffer, MAX_SIZE, 0);
    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "Receive failed with error: " << WSAGetLastError() << std::endl;
        //closesocket(socket_);
        WSACleanup();
        return SOCKET_ERROR;
    }
    else if (bytesReceived == 0) {
        std::cerr << "Connection closing...\n";
    }
    return bytesReceived;
}
