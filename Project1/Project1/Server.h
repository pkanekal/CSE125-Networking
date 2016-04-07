#pragma once

#include <string>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

class Server
{

private:
	int clientSocket;
	int listenSocket;
	std::string port;

	int setupSocket(std::string port);
	int acceptTCPConnection(int listenSocket);
	void handleClient(int clientSocket);
	//std::string encodeMessage(std::string msg);

public:
	Server(std::string port);
	void start();
	~Server();
};

