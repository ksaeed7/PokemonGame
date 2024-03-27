// LIST Command.hpp
#include <set>
#pragma once

#include "ICommand.hpp"
#include "../UserClass/UserClass.h"
#include "../../Dependencies/DependencyInjectionContainer.h"
using namespace std;

class DepositCommand : public ICommand {
public:
    DepositCommand(User& user, Authenticator& auth, NetworkHandler& net) :
        user_(user),
        authenticator_(auth),
        networkHandler_(net) {}

    virtual bool execute(SOCKET clientSocket, const string& command_) override {

        if (!validateCommand(command_)) {
            //statusCode = 404;
           //statusMessage = "COMMAND NOT FOUND!";
            //int msg = networkHandler_.peek(user_.getIpAddress());
            //if (msg == 200)
                //networkHandler_.sendData(statusCode, statusMessage);
            networkHandler_.sendData(statusCode, statusMessage);
            return false;
        }
        else
        {
            //cout << amount<<endl;
            //cout << authenticator_.getAuthenticatedUserId() <<endl;
            updatedBalance = user_.deposit(authenticator_.getAuthenticatedUserId(), amount, statusCode, statusMessage); //user_.listItems(authenticator_.getAuthenticatedUserId(), authenticator_.getRootUser());
            string msg = "";
            if (authenticator_.getRootUser())
            {
                msg = "(Root) ";
            }
            msg = msg + user_.getUserName();
            cout << "FROM: " << user_.getIpAddress() << " Received DEPOSIT: " << msg << endl;
            
            
            if (statusCode == 200)
            {
                string newMessage = "Deposit Successfully. New User balance $" + to_string(updatedBalance);
                networkHandler_.sendData(newMessage.c_str());
            }
            else
            {
                networkHandler_.sendData(statusCode, statusMessage);
            }

            return true;
        }
        //bool value = authenticator_.authenticateUser(username, password, statusCode, statusMessage);

        //return value;
        return 0;
    }
private:

    User& user_;
    int statusCode;
    string statusMessage;
    string samount;
    double amount;
    string cmd;
    double updatedBalance = 0;
    vector <Card> cards_;
    virtual bool validateCommand(const string& buffer) override {

        std::stringstream ss(buffer);
        std::set<std::string> validCommandTypes = { "DEPOSIT" };

        // Validate command type


        if (!(ss >> cmd >> samount)) {
            statusCode = 403;
            statusMessage = "ERROR: INVALID COMMAND or AMOUNT!";
            return false;
        }

        if (validCommandTypes.count(cmd) == 0) {
            cout << "Invalid command type received: " << cmd << std::endl;
            statusCode = 404;
            statusMessage = "ERROR: INVALID COMMAND!";

            return false;
        }
        try {
            amount = std::stod(samount);
            if (amount <= 0 || amount > 10000000)
            {
                statusCode = 403;
                statusMessage = "Error: amount is invalid";
                return false;
            }
        }
        catch (const std::invalid_argument&) {
            statusCode = 403;
            statusMessage = "ERROR: INVALID VALUE!";
            return false;
        }
        catch (const std::out_of_range&) {
            statusCode = 403;
            statusMessage = "ERROR: OUT OF RANGE";
            return false;

        }


        return true;
    }

   
    Authenticator& authenticator_;
    NetworkHandler& networkHandler_;
};

