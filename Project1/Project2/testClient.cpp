#include "Client.h"
#include <iostream>

int main() {
	Client* c = new Client("127.0.0.1", "5000");

	c->sendMessage("string1");
	//c->GetStatus(std::string("After SendMessage"));

	std::string response = c->receiveMessage();
	std::cout << "Received a response!" << std::endl;
	std::cout << response << std::endl;
	//c->GetStatus(std::string("After Response"));
	sleep(10);
	c->sendMessage("string2");
	response = c->receiveMessage();
	std::cout << "Received a response!" << std::endl;
	std::cout << response << std::endl;

	delete c;

	getchar();
	return 1;
}