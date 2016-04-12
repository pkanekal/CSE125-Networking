#include "Client.h"
#include <iostream>
#include <Windows.h>

int main() {
	Client* c = new Client("127.0.0.1", "5000");

	c->sendMessage("string1");
	std::string response = c->receiveMessage();
	std::cout << "Received a response!" << std::endl;
	std::cout << response << std::endl;
	//Sleep(10000);

	Client* c0 = new Client("127.0.0.1", "5000");
	c0->GetStatus("c0");
	c0->sendMessage("c0string1");
	response = c0->receiveMessage();
	std::cout << "Received a response!" << std::endl;
	std::cout << response << std::endl;

	c->sendMessage("string2");
	response = c->receiveMessage();
	std::cout << "Received a response!" << std::endl;
	std::cout << response << std::endl;

	delete c;
	c0->sendMessage("c0string2");
	response = c0->receiveMessage();
	std::cout << "Received a response!" << std::endl;
	std::cout << response << std::endl;
	delete c0;

	getchar();
	return 1;
}