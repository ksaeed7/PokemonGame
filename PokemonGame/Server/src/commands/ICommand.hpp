// ICommand.hpp
#ifndef ICOMMAND_HPP
#define ICOMMAND_HPP

#include <string>
#include <WinSock2.h>

class ICommand {
public:
    virtual ~ICommand() = default;

    virtual bool execute(SOCKET clientSocket, const std::string &buffer) = 0;
private:
    virtual bool validateCommand(const std::string& buffer) = 0;
};

#endif // ICOMMAND_HPP