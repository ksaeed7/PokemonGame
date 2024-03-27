#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mutex>
#include <stdio.h>
#include <string>
#include <iostream>
#include <set>

#include "IServer.h"
#include "../UserClass/UserClass.h" 
using namespace std;

#define MAX_PENDING1 500
class Server : public IServer {
public:
    Server(int port);

    ~Server();

    SOCKET acceptClient(struct sockaddr_in& clientAddr, int& clientLen);
    //void addActiveUser(const User& user);
    std::set<User> activeUsers;
    std::mutex activeUsersMutex;
    //void removeActiveUser(const User& user);
    void addActiveUser(const User user) override;
    void removeActiveUser(const User user) override;
    string listActiveUsers() const override;
    void shutdown() override;
    bool shouldShutdown() const;

private:
    int port_;
    WSADATA wsaData_;
    SOCKET serverSocket_;
    atomic_bool shutdownServer;
    struct sockaddr_in serverAddr_;
};
