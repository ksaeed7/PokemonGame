#pragma once

#include "../../include/sqlite3.h"
#include <string>
#include <mutex>
#include <iostream>
#include <vector>
#include "../Card.hpp"
using namespace std;

class db {
private:
    
    sqlite3* connection;
    mutex dbMutex;

    string users_table_sql = "CREATE TABLE IF NOT EXISTS Users ( "
        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "first_name TEXT NOT NULL, "
        "last_name TEXT NOT NULL, "
        "user_name TEXT NOT NULL, "
        "password TEXT, "
        "usd_balance DOUBLE NOT NULL, "
        "is_root BOOLEAN NOT NULL DEFAULT FALSE"
        ");";
    string pokemon_cards_table_sql = "CREATE TABLE IF NOT EXISTS Pokemon_cards ("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "card_name TEXT NOT NULL,"
        "card_type TEXT NOT NULL," 
        "rarity TEXT NOT NULL, count INTEGER,"
        "owner_id INTEGER,"
        "FOREIGN KEY(owner_id)" 
        "REFERENCES Users(ID)); ";

    void create_database(string database_name);
    void validateUsersDB();
    void initializePokemonCards();

public:
    db(const std::string& dbPath);
    ~db();
    bool getBalanceByOwnerId(int owner_id, double& balance, std::string& first_name, std::string& last_name, int& errorCode, std::string& errorMsg);
    double deposit(int owner_id, double amount, int& errorCode, std::string& errorMsg);
    void addUser(string first_name, string last_name, string user_name, string password, bool isRoot);
    bool userExists(const std::string& username, const std::string& password, int& user_id, bool& isRootUser, std::string& first_name, std::string& last_name);
    vector<Card> lookup(const std::string& query, int& errorCode, std::string& errorMsg);
    void sellCard(string card_name, int quantity, double price, int ownerID, int& errorCode, string& errorMsg);
    vector<Card> getCardsByOwnerId(int owner_id, bool isRootUser);
    void buyCard(string card_name, string card_type, string rarity, double price_per_card, int num_cards_to_buy, int buyerID, int &errorCode, string &errorMessage);
    sqlite3* getConnection() const;
    string login(const std::string& username, const std::string& password);
    template <typename Func>
    auto executeWithLock(Func&& func) -> decltype(func()) {
        std::lock_guard<std::mutex> lock(dbMutex);
        return func();
    }
};