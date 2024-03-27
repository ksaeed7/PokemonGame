
#include "UserClass.h"

User::User(const std::string& username, const std::string& ipAdd, db& databaseInstance)
    : username_(username), ipAddress(ipAdd), database_(databaseInstance) {}

std::string User::getUserName() const {
    return username_;
}
void User::setUserName(string username)  {
    username_ = username;
}
string User::getIpAddress() const {
    return ipAddress;
}

bool User::getBalance(int owner_id, double& balance, std::string& first_name, std::string& last_name, int& errorCode, std::string& errorMsg)
{
    return database_.getBalanceByOwnerId(owner_id, balance, first_name, last_name, errorCode, errorMsg);
}
void User::buyItem(string name, string type, string rarity, double price, int count, int ownerID, int& errorCode, string& errorMsg)
{
    database_.buyCard(name, type, rarity, price, count, ownerID, errorCode, errorMsg);
}

void User::sellItem(string card_name, int quantity, double price, int ownerID, int& errorCode, string& errorMsg) {
    database_.sellCard(card_name, quantity, price, ownerID, errorCode, errorMsg);
}

double User::deposit(int owner_id, double amount, int& errorCode, std::string& errorMsg) {
    return database_.deposit(owner_id, amount, errorCode, errorMsg);
}

vector <Card> User::lookup(const std::string& query, int& errorCode, std::string& errorMsg)
{
    return database_.lookup(query, errorCode, errorMsg);
}
vector<Card> User::listItems(int ownerId, bool isRootUser)
{
    return database_.getCardsByOwnerId(ownerId, isRootUser);
}