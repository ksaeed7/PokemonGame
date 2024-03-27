#include "clientHandler.h"
#include "../ClientSession/ClientSession.h"



ClientHandler::ClientHandler(SOCKET clientSocket, db& database, IServer &server)
    : clientSocket_(clientSocket), database_(database), server_(server){
    
}

void ClientHandler::handle() {
    char buffer[MAX_LINE];

    while (clientSocket_ != INVALID_SOCKET)
    {
        
        if (clientSocket_ == INVALID_SOCKET) {
            // Handle invalid socket
            cout << "Socket is invalid." << endl;
            closesocket(clientSocket_);
            break;
            exit;
        }
        memset(buffer, 0, MAX_LINE);
        int bytesReceived = recv(clientSocket_, buffer, MAX_LINE, 0);
        int returnValue = -1;
        //int bytesReceived = networkHandler.receiveData(buffer);

        if (bytesReceived <= 0) {
            // Handle error or client disconnect
            int bytesReceived = recv(clientSocket_, buffer, MAX_LINE, 0);
        }

        //DependencyInjectionContainer dic;
        

        std::string commandType, username, password;

        if (!getCommandType(buffer, commandType, username, password)) {
            // Handle invalid command format
           // closesocket(clientSocket_);
            //cout << "closed here2" << endl;
            //return;
        }

        if (commandType == "LOGIN") {

            NetworkHandler networkHandler(clientSocket_);
            Authenticator authenticator(database_);
            //DependencyInjectionContainer dic(database_, user, networkHandler, authenticator);
            int statusCode;
            std::string statusMessage;
            struct sockaddr_in addr;
            int len = sizeof(addr);
            char ipstr[INET6_ADDRSTRLEN];
            if (getpeername(clientSocket_, (struct sockaddr*)&addr, &len) == -1) {
                cerr << "getpeername failed with error: " << WSAGetLastError() << endl;
            }
            else {
                inet_ntop(AF_INET, &(addr.sin_addr), ipstr, sizeof ipstr);
                cout << "Client IP address: " << ipstr << endl;
            }
            LoginCommand loginCommand(authenticator, networkHandler);
            if (loginCommand.execute(clientSocket_, buffer)) {
                // If login is successful, create a client session and proceed to process other commands
                User user(username, ipstr, database_);

                server_.addActiveUser(user);
                cout << "Command Received: " << commandType << " from " << ipstr << std::endl;
                ClientSession session(clientSocket_, user, authenticator, networkHandler, server_);

                returnValue = session.processCommands();
                //session.~ClientSession();
                server_.removeActiveUser(user);
                //cout << "ReturnValue" << returnValue << endl;
                if (returnValue == 400)
                {

                    //cout << "closed here4" << endl;
                    //server_.addActiveUser(user);
                    closesocket(clientSocket_);
                    return;
                }
                else if (returnValue == 500) {
                    cout << "Shutdown command received, shutting down server." << endl;
                    closesocket(clientSocket_);
                    server_.shutdown();
                    return;
                }
            }
            else
            {
                cout << "INVALID";
            }
            
        }
        if (commandType == "QUIT")
        {
            //cout << "closed here3" << endl;
            //server_.addActiveUser(user);
            closesocket(clientSocket_);
            break;
        }
        
    }
    // Handle other command types here...*/
}

bool ClientHandler::getCommandType(const std::string& buffer, std::string& commandType, std::string& username, std::string& password) {
    std::istringstream iss(buffer);
    iss >> commandType;

    // List of valid command types
    std::set<std::string> validCommandTypes = { "LOGIN", "SIGNUP" };

    // Validate command type
    if (validCommandTypes.count(commandType) == 0) {
        //cout << "Invalid command type received: " << commandType << std::endl;
        return false;
    }

    if (commandType == "LOGIN") {
        iss >> username >> password;
        //if (username.empty() || password.empty()) {
            //cout<< "Invalid LOGIN command format. Missing username or password." << std::endl;
            //send(clientSocket_, "Error: No username and/or password.", 8, 0);
         //   return false;
       // }
    }
    // Handle other command types here...

    //std::cout << "Command Received: " << commandType << " from " << ipstr << std::endl;
    return true;
}