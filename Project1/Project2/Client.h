#pragma once
#include <string>

class Client
{
private:
	std::string serverIp;
	std::string port;
	bool ConnectionEstablished;
	int ConnectSocket;

	int SetupTCPConnection(std::string serverip, std::string port);

public:
	Client();
	Client(std::string ip, std::string port);
	//Establishes a Connection based on the ip and port
	//Returns 1 if failure

	//Requirements, need connection to be established... What should the data type be TODO:
	int sendMessage(std::string message);

	//Returns string with recieved
	std::string receiveMessage();

	//Manual Shutdown
	int CloseConnection();

	~Client();
};

