#include "Practical.h"
#ifdef __LINUX
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#endif
#include <iostream>

//TODO Make Linux compatible?
int decodeLength(std::string message){
	if (message.size() < 4){
		return 0;
	}

	char messageAsInt[5];
	std::cout << sizeof(int) << std::endl;
	memcpy_s(messageAsInt, sizeof(int), message.c_str(), sizeof(int));
	messageAsInt[4] = '\0';
	std::cout << "decodelength " << messageAsInt << std::endl;
	return ntohl((u_long)messageAsInt);
}