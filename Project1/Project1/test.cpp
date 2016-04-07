#include "Server.h"

int main() {
	Server s = Server("5000");
	s.start();
	return 1;
}