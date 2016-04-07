#pragma once
#include <string>

class Client
{
private:
	std::string serverip;
	std::string port;
	bool ConnectionEstablished;
	SOCKET ConnectSocket;

public:
	Client();
	//Establishes a Connection based on the ip and port
	//Returns 1 if failure
	int SetupTCPConnection(std::string serverip, std::string port);

	//Requirements, need connection to be established... What should the data type be TODO:
	int sendMessage(std::string message);

	//Returns string with recieved
	std::string Client::receiveMessage();

	//Manual Shutdown
	int CloseConnection();

	~Client();
};

