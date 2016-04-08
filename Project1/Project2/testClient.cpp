#include "Client.h"
#include <iostream>

int main() {
	Client c("127.0.0.1", "5000");
	c.sendMessage("sd");
	std::string response = c.receiveMessage();
	std::cout << "Received a response!" << std::endl;
	std::cout << response << std::endl;
	return 1;
}