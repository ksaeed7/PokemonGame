// LIST Command.hpp
#include <set>
#ifndef LIST_COMMAND_HPP
#define LIST_COMMAND_HPP

#include "ICommand.hpp"
#include "../UserClass/UserClass.h"
#include "../../Dependencies/DependencyInjectionContainer.h"
using namespace std;

class ListCommand : public ICommand {
public:
    ListCommand(User& user, Authenticator& auth, NetworkHandler& net) :
        user_(user),
        authenticator_(auth),
        networkHandler_(net) {}

    virtual bool execute(SOCKET clientSocket, const string& command_) override {

        if (!validateCommand(command_)) {
            statusCode = 404;
            statusMessage = "COMMAND NOT FOUND!";
            int msg = networkHandler_.peek(user_.getIpAddress());
            if (msg == 200)
                networkHandler_.sendData(statusCode, statusMessage);
            
            return false;
        }
        else
        {
            cards_ = user_.listItems(authenticator_.getAuthenticatedUserId(), authenticator_.getRootUser());
            string msg = "";
            if (authenticator_.getRootUser())
            {
                msg = "(Root) ";
            }
            msg = msg + user_.getUserName();
            cout << "FROM: " << user_.getIpAddress() << " Received LIST: " << msg << endl;
            sendCardData(cards_);

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
    string cmd;
    int ownerID;

    int quantity;
    double price;
    vector <Card> cards_; 
    virtual bool validateCommand(const string& buffer) override {

        std::stringstream ss(buffer);
        std::set<std::string> validCommandTypes = { "LIST" };

        // Validate command type


        if (!(ss >> cmd)) {
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
        
        return true;
    }

    std::string serializeCard(const Card& card) {
        std::stringstream ss;
        ss << to_string(card.getId()) << " ";
        ss <<  card.getName() << " ";
        ss <<card.getType() << " ";
        ss << card.getRarity() << " ";
        ss << to_string(card.getCount() )<< " ";
        //ss << "Owner ID: " << card.getOwnerId() << "\n";
        ss << card.getOwnerName() << "\n";
        return ss.str();
    }

    void sendCardData(const std::vector<Card>& cards) {
        string listStart = "<LIST>";
        networkHandler_.sendData(listStart.c_str());
        //cout << "CARD SIZE" << cards.size() << endl;
        for (const auto& card : cards) {
            std::string card_str = serializeCard(card);
            //cout << card_str <<endl;
            networkHandler_.sendData(card_str.c_str());
        }
        string num = "<END>";
        networkHandler_.sendData(num.c_str());
    }

    Authenticator& authenticator_;
    NetworkHandler& networkHandler_;
};

#endif // SELL_COMMAND_HPP
