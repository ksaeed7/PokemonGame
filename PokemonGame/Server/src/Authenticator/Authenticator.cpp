
#include "Authenticator.h"

Authenticator::Authenticator(db& database)
    : database_(database) {}

bool Authenticator::authenticateUser(const std::string& username, const std::string& password, int& statusCode, std::string& statusMessage) {
    int user_id;
    bool rootUser;
    string firstName;
    string lastName;
    if (database_.userExists(username, password, user_id, rootUser, firstName, lastName)){
        statusCode = 200;
        statusMessage = "OK\nLogged In\n";
        authenticated_user_id_ = user_id;
        authRootUser = rootUser;
        authenticated = true;
        firstName_ = firstName;
        lastName_ = lastName;
        return true;
    }
    else {
        statusCode = 403;
        statusMessage = "Wrong UserID or Password\n";
        authenticated = false;
        return false;
    }
}

bool Authenticator::getAuth()
{
    return authenticated;
}

void Authenticator::logout()
{
    authRootUser = false;
    authenticated_user_id_ = NULL;
    firstName_ = "";
    lastName_ = "";
    authenticated = false;
}

bool Authenticator::getRootUser()
{
    return authRootUser;
}

int Authenticator::getAuthenticatedUserId() const {
    return authenticated_user_id_;
}

string Authenticator::getFirstName()
{
    return firstName_;
}

string Authenticator::getLastName()
{
    return lastName_;
}
