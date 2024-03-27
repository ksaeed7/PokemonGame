// Authenticator.h
#pragma once


#include "../Database/db.h"
#include <string>

class Authenticator {
public:
    Authenticator(db& database);
    bool authenticateUser(const std::string& username, const std::string& password, int& statusCode, std::string& statusMessage);
    bool getAuth();
    void logout();
    string getFirstName();
    string getLastName();
    int getAuthenticatedUserId() const;
    bool getRootUser();
private:
    db& database_;
    bool authRootUser = false;
    bool authenticated = false;
    string firstName_,lastName_;
    int authenticated_user_id_ = -1;
};

