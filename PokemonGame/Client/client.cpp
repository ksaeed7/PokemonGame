#include <iostream>
#include <cstring>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <sstream>
#include <vector>

#pragma comment(lib, "ws2_32.lib")
#define MAX_LINE 1024
using namespace std;
const int SERVER_PORT = 1352;  // Replace with the last 4 digits of your UM-ID
static const char* SERVER_IP = "127.0.0.1"; 

bool test = false;
std::string receiveFullMessage(SOCKET clientSocket, const std::string& endMarker) {
    std::string fullMessage;
    char buffer[MAX_LINE];
    //cout << "IN here" << endl;
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            std::cerr << "Error: Connection closed by server" << std::endl;
            return { fullMessage };
        }
        buffer[bytesReceived] = '\0';
        fullMessage += buffer;

        if (fullMessage.find(endMarker) != std::string::npos) {
            break;
        }
    }

    return fullMessage;
}

std::vector<std::string> split(const std::string& str, char delim) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}
/*
void processListData(const std::string& listData, int other) {
    std::vector<std::string> lines = split(listData, '\n');
    printf("%-5s %-15s %-10s %-10s %-10s\n", "ID", "Card Name", "Type", "Rarity", "Count");
    for (const std::string& line : lines) {
        std::istringstream ss(line);
        std::string id, name, type, rarity, count, ownersName;
        std::getline(ss, id, ' ');
        std::getline(ss, name, ' ');
        std::getline(ss, type, ' ');
        std::getline(ss, rarity, ' ');
        std::getline(ss, count, ' ');
        //std::getline(ss, ownersName);
        printf("%-5s %-15s %-10s %-10s %-10s\n",
            id.c_str(), name.c_str(), type.c_str(),
            rarity.c_str(), count.c_str());
    }
}*/

void processListData(const std::string& listData) {
    std::vector<std::string> lines = split(listData, '\n');
    printf("%-5s %-15s %-10s %-10s %-10s %-10s\n", "ID", "Card Name", "Type", "Rarity", "Count", "Owner Name");
    for (const std::string& line : lines) {
        std::istringstream ss(line);
        std::string id, name, type, rarity, count, ownersName;
        std::getline(ss, id, ' ');
        std::getline(ss, name, ' ');
        std::getline(ss, type, ' ');
        std::getline(ss, rarity, ' ');
        std::getline(ss, count, ' ');
        std::getline(ss, ownersName);
        printf("%-5s %-15s %-10s %-10s %-10s %-10s\n",
            id.c_str(), name.c_str(), type.c_str(),
            rarity.c_str(), count.c_str(), ownersName.c_str());
    }
}
SOCKET initializeAndConnectSocket(const char* serverIp, int port) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Error: WSAStartup failed" << std::endl;
        return INVALID_SOCKET;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Error: Unable to create client socket. Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return INVALID_SOCKET;
    }

    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(static_cast<uint16_t>(port));
    if (inet_pton(AF_INET, serverIp, &serverAddr.sin_addr) <= 0) {
        std::cerr << "Error: Invalid server IP address" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return INVALID_SOCKET;
    }

    if (connect(clientSocket, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Error: Connection failed. Error: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return INVALID_SOCKET;
    }

    return clientSocket;
}
const char* serverIp = "127.0.0.1";
int port = 1352;

bool login = false;

void awaitLogin(SOCKET clientSocket)
{
    std::string command;
    char buffer[1024];
    while (true)
    {
        std::cout << "Enter LOGIN COMMAND: ";
        std::string command;
        char buffer[1024];
        std::getline(std::cin, command);
        std::string input = command + "\n";
        if (command.find("LOGIN") == 0)
        {
            send(clientSocket, input.c_str(), input.size(), 0);

            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

            if (bytesReceived > 0)
            {
                string response(buffer);
                if (response.find("200") != string::npos)
                {
                    login = true;
                    //cout << "HERE"<<endl;

                    //buffer[bytesReceived] = '\0';

                    buffer[bytesReceived] = '\0';
                    std::cout << buffer << std::endl;
                    break;
                }
                buffer[bytesReceived] = '\0';
                std::cout << buffer << std::endl;
            }
            //cout << "Incorrect LOGIN COMMAND!" << endl;
        }
        else
        {
            cout << "Incorrect COMMAND!" << endl;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Using Local IP address" << std::endl;
    }
    else {
        serverIp = argv[1];
        port = std::stoi(argv[2]);
    }
    SOCKET clientSocket = initializeAndConnectSocket(serverIp, port);
    if (clientSocket == INVALID_SOCKET) {
        return 1;
    }

    char buffer[1024];
    int bytesReceived;
    

    
    awaitLogin(clientSocket);
    while (true) {
        std::string command;
        std::cout << "Enter command (LOGOUT, BUY, SELL, LIST, BALANCE, WHO, LOOKUP DEPOSIT SHUTDOWN, QUIT): ";
        std::getline(std::cin, command);
        std::string input = command + "\n";
        send(clientSocket, input.c_str(), input.size(), 0);
        
        if ((command.find("LIST") == 0)) {
            std::string receivedData = receiveFullMessage(clientSocket, "<END>");
            if (receivedData.empty()) {
                continue;
            }
            size_t listStart = receivedData.find("<LIST>");
            size_t listEnd = receivedData.find("<END>");
            if (listStart != std::string::npos && listEnd != std::string::npos && listStart < listEnd) {
                std::string listData = receivedData.substr(listStart + 6, listEnd - (listStart + 6));
                processListData(listData);
            }
        }
        else if ((command.find("LOOKUP")) == 0)
        {
            std::string receivedData1 = receiveFullMessage(clientSocket, "<END>");
            if (receivedData1.empty()) {
                continue;
            }
            size_t listStart1 = receivedData1.find("<LIST>");
            size_t listEnd1 = receivedData1.find("<END>");
            if (listStart1 != std::string::npos && listEnd1 != std::string::npos && listStart1 < listEnd1) {
                std::string listData1 = receivedData1.substr(listStart1 + 6, listEnd1 - (listStart1 + 6));
                processListData(listData1);
            }
        }
        else if (command == "LOGOUT") {
            bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            buffer[bytesReceived] = '\0';
            std::cout << "Response to LOGOUT: " << buffer << std::endl;
            login = false;
            awaitLogin(clientSocket);
        }
        else {
            //cout << "IN HERE"<< endl;
            bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            
            if (bytesReceived <= 0 && (command.find("QUIT"))) {

                if(login)
                    std::cerr << "Unable to authenticate, please log back in!" << std::endl;
                else
                    std::cerr << "Invalid LOGIN Username or Password" << std::endl;
                clientSocket = initializeAndConnectSocket(serverIp, port);
                continue;
            }
            else
            {
                login = true;
                //cout << "HERE"<<endl;
            }
            buffer[bytesReceived] = '\0';

            std::cout << buffer << std::endl;
        }

        /*if (command == "LOGOUT") {
            closesocket(clientSocket);
            WSACleanup();
            clientSocket = initializeAndConnectSocket(serverIp, port);
            if (clientSocket == INVALID_SOCKET) {
                break;
            }
        }*/
        if (command == "QUIT") {
            break;
        }
    }

    closesocket(clientSocket);
    WSACleanup();
    std::cout << "Client closed." << std::endl;
    return 0;
}


/*else if (receivedData.find("LIST") != std::string::npos) {
                //printf("%-5s %-15s %-10s %-10s %-10s %-10s\n", "ID", "Card Name", "Type", "Rarity", "Count", "Owner Name");
                //while (true) {
                    bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
                    if (bytesReceived <= 0) {
                        std::cerr << "Error: Connection closed by server" << std::endl;
                        break;
                    }
                    buffer[bytesReceived] = '\0';
                    std::string bufferStr(buffer, bytesReceived);

                    // Check for terminating string
                    if (bufferStr.find("5675675") != std::string::npos) {
                        cout << "FOUND END" << endl;
                        break;
                    }
                    else {
                        string id, name, type, rarity, count, ownersName;

                        std::istringstream ss(bufferStr);

                        std::getline(ss, id, ' ');
                        std::getline(ss, name, ' ');
                        std::getline(ss, type, ' ');
                        std::getline(ss, rarity, ' ');
                        std::getline(ss, count, ' ');
                        std::getline(ss, ownersName); // Assuming owner name is the last element in the line.

                        printf("%-5s %-15s %-10s %-10s %-10s %-10s\n", id.c_str(), name.c_str(), type.c_str(), rarity.c_str(), count.c_str(), ownersName.c_str());
                    }
                }(
            }*/
/*while (true) {
        // Get username and password from the user
        std::string username, password;
        std::cout << "Enter username: ";
        std::getline(std::cin, username);
        std::cout << "Enter password: ";
        std::getline(std::cin, password);

        std::string input = "LOGIN " + username + " " + password + "\n";

        // Send login command to the server
        send(clientSocket, input.c_str(), input.size(), 0);

        // Receive response from the server
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer)-1, 0);
        if (bytesReceived <= 0) {
            std::cerr << "Error: Connection closed by server" << std::endl;
            break;
        }

        buffer[bytesReceived] = '\0';
        std::cout << buffer << std::endl;

        // Check if the client should terminate
        if (input == "SHUTDOWN" || input == "QUIT") {
            break;
        }
        std::string firstName, lastName;

        // Check if the server requests the user's first and last name
        if (strstr(buffer, "701 Please provide your first name and last name") != nullptr) {

            std::cout << "Enter your first name: ";
            std::getline(std::cin, firstName);
            std::cout << "Enter your last name: ";
            std::getline(std::cin, lastName);

            // Send the first name and last name to the server
            std::string nameData = firstName + " " + lastName +"\n";
            send(clientSocket, nameData.c_str(), nameData.size(), 0);

            //cout << "Data SENT" << endl;
            // Receive and display the response from the server
            bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            //cout << bytesReceived << endl;
            if (bytesReceived <= 0) {
                std::cerr << "Error: Connection closed by server" << std::endl;
                break;
            }


            buffer[bytesReceived] = '\0';
            std::cout << buffer << std::endl;

            string strBuffer(buffer);

            if (strBuffer.find("200") == 0) {
                string msg = firstName + " " + lastName + " has been added to database";
                esc = true;
                break;
            }
        }
        else
        {
            //cout << "Entered";
            break;
        }

        if (esc)
            break;
    }
    */