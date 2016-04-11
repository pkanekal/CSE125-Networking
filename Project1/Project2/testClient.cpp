#include "Client.h"
#include <iostream>

int main() {
	Client* c = new Client("127.0.0.1", "5000");
	//getchar();
	c->sendMessage("sd");
	c->GetStatus(std::string("After SendMessage"));

	//getchar();
	std::string response = c->receiveMessage();
	std::cout << "Received a response!" << std::endl;
	std::cout << response << std::endl;
	c->GetStatus(std::string("After Response"));
	delete c;
	getchar();
	return 1;
}