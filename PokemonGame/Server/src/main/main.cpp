#include <stdio.h>
#include <string>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "../../include/sqlite3.h"
#include "../Server/Server.h"

#include <string>
#include <sstream>
#include <map>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <mutex>
#include <iomanip>

#include <memory>
#include "../Database/db.h"
#include <set>

#include "../ClientHandler/clientHandler.h"
#define SERVER_PORT 1352

using namespace std;

atomic_bool shutdownServer(false);


int main()
{
    db database("p2_db");
    int count = 0;

    try {
        Server server1(SERVER_PORT);

        std::thread shutdownThread([&]() {
            while (!server1.shouldShutdown()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));

            }
            shutdownServer = true;
            return;
            });

        while (!shutdownServer) {
            struct sockaddr_in clientAddr;
            int clientLen = sizeof(clientAddr);
            SOCKET clientSocket = server1.acceptClient(clientAddr, clientLen);
            if (clientSocket == INVALID_SOCKET && count < 10) {
                fprintf(stderr, "Accept failed with error: %d\n", WSAGetLastError());
                count++;
                continue;  // Continue accepting other connections
            }

            ClientHandler clientHandler(clientSocket, database,server1);
            std::thread clientThread(&ClientHandler::handle, std::ref(clientHandler));
            clientThread.detach();

        }

    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
