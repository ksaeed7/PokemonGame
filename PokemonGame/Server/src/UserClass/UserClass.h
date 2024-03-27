// UserClass.h
#pragma once

#include "../Database/db.h"
#include <string>

class User {
public:
    User(const std::string& username, const string& ipAddress, db& databaseInstance);
    std::string getUserName() const;
    string getIpAddress() const;
    void setUserName(string username);
    bool getBalance(int owner_id, double& balance, std::string& first_name, std::string& last_name, int& errorCode, std::string& errorMsg);
    void buyItem(string name, string type, string rarity, double price, int count, int ownerID, int &errorCode, string &errorMsg);
    void sellItem(string card_name, int quantity, double price, int ownerID, int& errorCode, string& errorMsg);
    vector<Card> listItems(int ownerId, bool isRootUser);
    vector <Card> lookup(const std::string& query, int& errorCode, std::string& errorMsg);
    bool operator<(const User& other) const {
        return username_ < other.username_;
    }
    double deposit(int owner_id, double amount, int& errorCode, std::string& errorMsg);
private:
    db& database_;  // Reference to the db instance
    std::string username_;
    std::string ipAddress;
};

