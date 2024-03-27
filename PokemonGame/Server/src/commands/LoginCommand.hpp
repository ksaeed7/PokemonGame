#pragma once

#include "ICommand.hpp"
#include "../NetworkHandler/NetworkHandler.h"
#include "../Authenticator/Authenticator.h"
class LoginCommand : public ICommand {
public:
    LoginCommand(Authenticator& auth, NetworkHandler& net)
        : authenticator_(auth),
        networkHandler_(net) {}

    virtual bool execute(SOCKET clientSocket, const string &buffer) override {
        std::stringstream ss(buffer);
        std::string cmd, username, password;
        ss >> cmd >> username >> password;
        
        if (!validateCommand(buffer)) {
            //send(clientSocket, "400 Bad Request\n", 16, 0);
            statusCode = 404;
            errorMsg = "ERROR: INVALID COMMAND!";
            cout << statusCode << errorMsg;
            networkHandler_.sendData(statusCode, errorMsg);
            return false;
        }
        

        bool value = authenticator_.authenticateUser(username, password, statusCode, errorMsg);
        networkHandler_.sendData(statusCode, errorMsg);
        return value;
    }
private:

    int statusCode;
    
    string errorMsg;
    

    virtual bool validateCommand(const string& buffer) override {

        std::stringstream ss(buffer);


        std::set<std::string> validCommandTypes = { "LOGIN" };

        string cmd, username, password;
        // Validate command type

        //FORMAT:
        // BUY type rarity price count ID

        if (!(ss >> cmd >> username >> password)) {
            statusCode = 403;
            errorMsg = "ERROR: INVALID COMMAND!";
            return false;
        }

        if (validCommandTypes.count(cmd) == 0) {
            cout << "Invalid command type received: " << cmd << std::endl;
            statusCode = 404;
            errorMsg = "ERROR: INVALID COMMAND!";

            return false;
        }
        
        if (username.empty() || password.empty())
        {
            statusCode = 404;
            errorMsg = "ERROR: Wrong Input";
            return false;
        }
        return true;
    }
    Authenticator& authenticator_;
    NetworkHandler& networkHandler_;
};

//#endif // LOGIN_COMMAND_HPP
