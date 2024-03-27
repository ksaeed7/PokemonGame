// BUYCommand.hpp
#include <set>
#ifndef SELL_COMMAND_HPP
#define SELL_COMMAND_HPP

#include "ICommand.hpp"
#include "../UserClass/UserClass.h"
#include "../../Dependencies/DependencyInjectionContainer.h"
using namespace std;

class SellCommand : public ICommand {
public:
    SellCommand(User& user, Authenticator& auth, NetworkHandler& net) :
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
            user_.sellItem(pokemonName, quantity, price, ownerID, statusCode, statusMessage);
            cout << "FROM: " << user_.getIpAddress() << " Received Sell: " << pokemonName << " " << quantity << " " << price << " " << price << " " << ownerID << endl;
            networkHandler_.sendData(statusCode, statusMessage);
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
    string cmd, pokemonName, type, rarity, owner;
    string sprice;
    string sQuantity;
    string errorMsg;
    int ownerID;
    
    int quantity;
    double price;

    virtual bool validateCommand(const string& buffer) override {

        std::stringstream ss(buffer);


        std::set<std::string> validCommandTypes = { "SELL" };

        // Validate command type

        //FORMAT:
        // BUY type rarity price count ID

        if (!(ss >> cmd >> pokemonName >> sQuantity >> sprice >> owner)) {
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
            quantity = std::stoi(sQuantity);
            price = std::stod(sprice);
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

#endif // SELL_COMMAND_HPP
