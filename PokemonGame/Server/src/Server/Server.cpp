#include "Server.h"

Server::Server(int port) : port_(port) {
	// Initialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData_) != 0) {
		fprintf(stderr, "WSAStartup failed with error: %d\n", WSAGetLastError());
		throw std::runtime_error("WSAStartup failed");
	}

	serverSocket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket_ == INVALID_SOCKET) {
		fprintf(stderr, "Socket creation failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		throw std::runtime_error("Socket creation failed");
	}

	memset(&serverAddr_, 0, sizeof(serverAddr_));
	serverAddr_.sin_family = AF_INET;
	serverAddr_.sin_addr.s_addr = INADDR_ANY;
	serverAddr_.sin_port = htons(port_);

	if (bind(serverSocket_, (struct sockaddr*)&serverAddr_, sizeof(serverAddr_)) == SOCKET_ERROR) {
		fprintf(stderr, "Bind failed with error: %d\n", WSAGetLastError());
		closesocket(serverSocket_);
		WSACleanup();
		throw std::runtime_error("Bind failed");
	}

	if (listen(serverSocket_, MAX_PENDING1) == SOCKET_ERROR) {
		fprintf(stderr, "Listen failed with error: %d\n", WSAGetLastError());
		closesocket(serverSocket_);
		WSACleanup();
		throw std::runtime_error("Listen failed");
	}
}

Server::~Server() {
	closesocket(serverSocket_);
	WSACleanup();
}

SOCKET Server::acceptClient(struct sockaddr_in& clientAddr, int& clientLen) {
	return accept(serverSocket_, (struct sockaddr*)&clientAddr, &clientLen);
}

 void Server::addActiveUser(const User user) {
    std::lock_guard<std::mutex> lock(activeUsersMutex);
    activeUsers.insert(user);
}

void Server::removeActiveUser(const User user) {
    std::lock_guard<std::mutex> lock(activeUsersMutex);
    activeUsers.erase(user);
}

string Server::listActiveUsers() const {
	string msg = "";
	for (const auto& user : activeUsers) {
		msg = msg + user.getUserName() + " " + user.getIpAddress() + "\n";
	}
	return msg;
}

void Server::shutdown() {
	cerr << "Server is shutting down." << endl;
	shutdownServer = true;
}

bool Server::shouldShutdown() const {
	return shutdownServer.load();
}