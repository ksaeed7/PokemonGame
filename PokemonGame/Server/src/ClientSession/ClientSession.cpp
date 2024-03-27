#include "ClientSession.h"
#include <future>
#include "../commands/LoginCommand.hpp"


#define MAX_LINE 1024

using namespace std;

ClientSession::ClientSession(SOCKET& clientSocket, User& user, Authenticator& auth, NetworkHandler& networkHandler, IServer &server) : 
    clientSocket_(clientSocket),
    user_(user),
    authenticator_(auth),
    networkHandler_(networkHandler),
    server_(server),
    buyCMD(user, auth, networkHandler),
    sellCMD(user, auth, networkHandler),
    listCMD(user, auth, networkHandler),
    lookupCMD(user, auth, networkHandler),
    depositCMD(user, auth, networkHandler),
    balanceCMD(user, auth, networkHandler),
    active(true),
    timerReset(true),
    waiting(false)
{ 
   sessionTimer = std::thread(&ClientSession::sessionTimeout, this);
}


ClientSession::~ClientSession() {
    if (sessionTimer.joinable()) {
        active = false;
        if (waiting) {  // Check if sessionTimeout is waiting
            timerCV.notify_all();  // Notify timerCV to break out of the waiting state
        }
        sessionTimer.join();
    }
}

int ClientSession::processCommands() {
    char buffer[MAX_LINE];
    bool logout = false;
    bool destroySession = false;
    while (active) {
        
        std::future<int> future = std::async(std::launch::async, [this, &buffer]() {
            memset(buffer, 0, MAX_LINE);
            return recv(clientSocket_, buffer, MAX_LINE, 0);
            });

        std::future_status status;
        do {
            status = future.wait_for(std::chrono::milliseconds(10));
            if (!active) {
                if (waiting) {  // Check if sessionTimeout is waiting
                    timerCV.notify_all();  // Notify timerCV to break out of the waiting state
                }
                break;
            }
        } while (status != std::future_status::ready);

        if (!active) {
            break;
        }
        int statusCode;
        string statusMessage;
        int bytesReceived = future.get();
        //cout << "entry" << endl;
        if (bytesReceived <= 0 || (!validateBuffer(buffer))) {
             statusCode = 400;
             statusMessage = "ERROR: COMMAND";
             //cout << "Error" << endl;
            //int msg = networkHandler_.peek(user_.getIpAddress());
            //if (msg == 200)
                networkHandler_.sendData(statusCode, statusMessage);
                destroySession = true;
                
            break;
        }

        // Process received data
        string cmdType = getCommandType(buffer, statusCode, statusMessage);
        if (cmdType.empty()) {
            //int msg = networkHandler_.peek(user_.getIpAddress());
            //if (msg == 200)
                networkHandler_.sendData(statusCode, statusMessage);
                destroySession = true;
            break;
        }
        else {
            unique_lock<mutex> lock(timerMutex);  // Lock the mutex
            //cout << "Timer thread is waiting" << std::endl;
            while (!waiting && active) {
                // Wait until the timer thread is actually waiting
                lock.unlock();
                this_thread::sleep_for(chrono::milliseconds(1));
                lock.lock();
            }
            //cout << "Resetting timer" << std::endl;
            timerReset = true;
            timerCV.notify_one();  // Notify the timer thread
            lock.unlock();
            //cout << cmdType;
            if (authenticator_.getAuth())
            {
                if (cmdType == "BUY") {
                    buyCMD.execute(clientSocket_, buffer);
                }
                else if (cmdType == "SELL") {
                    sellCMD.execute(clientSocket_, buffer);
                }
                else if (cmdType == "LIST") {
                    listCMD.execute(clientSocket_, buffer);
                }
                else if (cmdType == "BALANCE") {
                    balanceCMD.execute(clientSocket_, buffer);
                }
                else if (cmdType == "WHO")
                {
                    string msg;
                    if (authenticator_.getRootUser())
                        msg = server_.listActiveUsers();
                    else
                        msg = "403 Command requires elevated access!";
                    networkHandler_.sendData(msg.c_str());
                }
                else if (cmdType == "DEPOSIT")
                {
                    depositCMD.execute(clientSocket_, buffer);
                }
                else if (cmdType == "LOOKUP")
                {
                    lookupCMD.execute(clientSocket_, buffer);
                }
                else if (cmdType == "LOGOUT") {
                    string msg = "200 Logout Successful";
                    networkHandler_.sendData(msg.c_str());
                    authenticator_.logout();
                    server_.removeActiveUser(user_);
                    logout = true;
                }
                else if (cmdType == "LOGIN")
                {
                    string msg = "A user is already logged in";
                    networkHandler_.sendData(msg.c_str());
                }
            }
            else
            {
                if (cmdType == "LOGIN")
                {
                    LoginCommand loginCommand(authenticator_, networkHandler_);
                    if (loginCommand.execute(clientSocket_, buffer))
                    {
                        std::stringstream ss(buffer);
                        std::string cmd, username;
                        ss >> cmd >> username;
                        user_.setUserName(username);
                        server_.addActiveUser(user_);
                    }
                }
            }
            if (cmdType == "QUIT")
            {
                cout << "QUIT received from IP: " << user_.getIpAddress() <<endl;
                return 400;
                break;
            }
            else if (cmdType == "SHUTDOWN" )
            {
                string msg;
                cout << "SHUTDOWN received from IP: " << user_.getIpAddress() << " name: " << user_.getUserName()<<endl;
                if (authenticator_.getRootUser() == true) {
                    msg = "Server has been shutdown!";
                    
                    
                    return 500;
                    break;
                }
                else
                {
                    msg = "403 failed: Command requires elevated access!";
                }
                networkHandler_.sendData(msg.c_str());
            }
        }
        
    }
    
    //networkHandler_.sendData(400, "Client time out!");
    if (!active)
    {
        cout << user_.getIpAddress() << " timed out!" << endl;
    }
    else
    {
        cout << user_.getIpAddress() << " logged out!" << endl;
    }
    //closesocket(clientSocket_);
    
    active = false;
    if (logout)
        return 200;
    else if (destroySession)
        return 400;
    return 100;
}



//this ensures integers are meant to be integers and strings are meant to be strings.
//it also ensures no weird inputs inbetween strings and values. 
bool ClientSession::validateBuffer(const string& buffer) {
    std::istringstream iss(buffer);
    std::string word;
    int count = 0;
    int max_iterations = 1000;
    bool isValid = true;
    while (iss >> word && count < max_iterations) {
        std::istringstream word_iss(word);
        double num;
        if (word_iss >> num) {
            std::string remaining;
            word_iss >> remaining;
            //cout << num << " " << remaining << endl;
            if (!remaining.empty())
                isValid = false;
        }
        count++;
    }
    return isValid && (count < max_iterations);
}

string ClientSession::getCommandType(const std::string& buffer, int &statusCode, string &statusMessage) {
    std::istringstream iss(buffer);
    string commandType;
    iss >> commandType;
    if (validCommandTypes.count(commandType) == 0) {
        statusCode = 404;
        statusMessage = "ERROR: INVALID COMMAND " + commandType;
        return std::string();
    }

    return commandType;
}

void ClientSession::sessionTimeout() {
    std::unique_lock<std::mutex> lock(timerMutex);
    while (active) {
        timerReset = false;
        waiting = true;  
        //cout << "Timer thread waiting" << std::endl;
        if (!timerCV.wait_for(lock, std::chrono::minutes(15), [this] { return timerReset; })) {
            //cout << "Timer expired. Closing" << std::endl;
            active = false;
        }
        //else {
            //std::cout << "Timer reset" << std::endl;
        
        waiting = false;  
    }
}