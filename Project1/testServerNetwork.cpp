#include "ServerNetwork.h"
#include "NetworkUtility.h"
#include <iostream>
#include <string>

int main() {
	ServerNetwork s("5000");
	s.start();
	return 0;
}