#pragma once
#include <winsock2.h>

#include <string>

class NetworkHandler {
public:
    NetworkHandler(SOCKET &socket);
    ~NetworkHandler();

    int sendData(const char* data);
    int receiveData(char* buffer);
    int sendData(int code, std::string message);
    int peek(std::string ip);
private:
    SOCKET &socket_;
    
};
