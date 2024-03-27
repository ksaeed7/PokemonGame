#pragma once
#include <string>


using namespace std;

//#include "../UserClass/UserClass.h"
class User;

class IServer {
public:
    
    virtual ~IServer() = default;
    virtual void addActiveUser(const  User user) = 0;
    virtual void removeActiveUser(const User user) = 0;
    virtual string listActiveUsers() const = 0;
    virtual void shutdown() = 0;
};