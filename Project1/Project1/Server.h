#pragma once
#include <string>

class Server
{

private:

	int setupSocket(std::string ip, std::string port);
	int acceptTCPConnection(int sock);

public:
	Server(std::string ip, std::string port);
	~Server();

};

