#include "Server.h"
#include "Practical.h"
#include <iostream>
#include <string>

int main() {
	Server s("5000");
	s.start();
	return 0;
}