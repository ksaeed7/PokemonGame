// BUYCommand.hpp
#include <set>
#ifndef BALANCE_COMMAND_HPP
#define BALANCE_COMMAND_HPP

#include "ICommand.hpp"
#include "../UserClass/UserClass.h"
#include "../../Dependencies/DependencyInjectionContainer.h"
#include <iomanip>
using namespace std;

class BalanceCommand : public ICommand {
public:
    BalanceCommand(User& user, Authenticator& auth, NetworkHandler& net) :
        user_(user),
        authenticator_(auth),
        networkHandler_(net) {}

    virtual bool execute(SOCKET clientSocket, const string& command_) override {

        if (!validateCommand(command_)) {
            statusCode = 404;
            statusMessage = "COMMAND NOT FOUND!";
            networkHandler_.sendData(statusCode, statusMessage);
            return false;
        }
        else
        {
            string firstName, lastName;
            bool valid = user_.getBalance(ownerID, balance, firstName, lastName , statusCode, statusMessage);
            cout << "FROM: " << user_.getIpAddress() << " Received Balance: " << ownerID << endl;
            //networkHandler_.sendData(statusCode, statusMessage);
            std::string msg = "Balance for user " + firstName + " " + lastName + ": $" + std::to_string(balance);
            std::ostringstream balance_stream;
            balance_stream << std::fixed << std::setprecision(2) << balance;
            std::string formatted_balance = balance_stream.str();
            std::string msg1 = "Balance for user " + firstName + " " + lastName + ": $" + formatted_balance;
            if (valid)
                networkHandler_.sendData(msg1.c_str());
            else

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
    string cmd, owner;
    string errorMsg;
    int ownerID;
    double balance;


    virtual bool validateCommand(const string& buffer) override {

        std::stringstream ss(buffer);


        std::set<std::string> validCommandTypes = { "BALANCE" };

        // Validate command type

        //FORMAT:
        // BUY type rarity price count ID

        if (!(ss >> cmd >> owner)) {
            statusCode = 403;
            statusMessage = "ERROR: INVALID COMMAND!";
            return false;
        }

        if (validCommandTypes.count(cmd) == 0) {
            cout << "Invalid command type received: " << cmd << std::endl;
            statusCode = 404;
            statusMessage = "ERROR: INVALID COMMAND!";

            return false;
        }
        try {
            ownerID = std::stoi(owner);
        }
        catch (const std::invalid_argument&) {
            statusCode = 403;
            statusMessage = "ERROR: INVALID INTEGER VALUE!";
            return false;
        }
        catch (const std::out_of_range&) {
            statusCode = 403;
            statusMessage = "ERROR: INTEGER OUT OF RANGE";
            return false;

        }
        return true;
    }
    Authenticator& authenticator_;
    NetworkHandler& networkHandler_;
};

#endif // BALANCE_COMMAND_HPP
