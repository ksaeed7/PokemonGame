#pragma once
#include <string>
#include <sstream>
#include <set>

//#include "../../Dependencies/DependencyInjectionContainer.h"

#include "../Authenticator/Authenticator.h"
#include "../NetworkHandler/NetworkHandler.h"
#include "../commands/BuyCommand.hpp"
#include "../commands/SellCommand.hpp"
#include "../commands/ListCommand.hpp"
#include "../commands/BalanceCommand.hpp"
#include "../Server/IServer.h"
#include "../commands/LookUpCommand.hpp"
#include "../commands/DepositCommand.hpp"

using namespace std;

class ClientSession {
public:
    ClientSession(SOCKET &clientSocket, User &user, Authenticator &auth, NetworkHandler &networkHandler, IServer &server);
    //void init();

    ~ClientSession();


    int processCommands();

private:
    SOCKET &clientSocket_;
    std::set<std::string> validCommandTypes = { "BUY", "SELL", "LIST", "BALANCE", "SHUTDOWN", "QUIT", "LOGOUT", "WHO", "LOGIN", "LOOKUP", "DEPOSIT"};
    User& user_;
    NetworkHandler& networkHandler_;
    Authenticator& authenticator_;

    //DependencyInjectionContainer* dic_;
    BuyCommand buyCMD;
    SellCommand sellCMD;
    ListCommand listCMD;
    LookUpCommand lookupCMD;
    DepositCommand depositCMD;
    BalanceCommand balanceCMD;
    IServer& server_;
    std::condition_variable timerCV;
    std::mutex timerMutex;
    bool waiting;
    bool timerReset;
    atomic<bool> active;
    std::thread sessionTimer;
    string ipAddress_;
    bool validateBuffer(const string& buffer);

    string getCommandType(const std::string& buffer, int& statusCode, string& statusMessage);

    void sessionTimeout();
};

