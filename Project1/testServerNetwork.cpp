#include "ServerNetwork.h"
#include "Practical.h"
#include <iostream>
#include <string>

int main() {
	ServerNetwork s("5000");
	s.start();
	return 0;
}