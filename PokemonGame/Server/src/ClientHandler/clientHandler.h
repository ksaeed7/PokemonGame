#pragma once
//#include "../Server/Server.h"
#include "../NetworkHandler/NetworkHandler.h"
#include "../Authenticator/Authenticator.h"
#include "../UserClass/UserClass.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <sstream>
#include <string>
#include <set>

#include "../commands/LoginCommand.hpp"
#include "../Server/IServer.h"

//class DependencyInjectionContainer;
#define MAX_LINE 1024
using namespace std;
class ClientHandler {
public:
    ClientHandler(SOCKET clientSocket, db& database, IServer &server);

    void handle();

private:
    SOCKET clientSocket_;
    db& database_;
    IServer& server_;
    
    bool getCommandType(const std::string& buffer, std::string& commandType, std::string& username, std::string& password);
    
};
