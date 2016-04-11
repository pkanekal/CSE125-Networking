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


#define DEFAULT_BUFLEN 512


int decodeContentLength(std::string message){
	int messageAsInt;
	if (message.size() < 4){
		return 0;
	}
	memcpy_s((void *) &messageAsInt, sizeof(int), message.c_str(), sizeof(int));
	return ntohl((u_long)messageAsInt);
}

char * encodeContentLength(std::string message) {
	char buf[DEFAULT_BUFLEN];
	memset(buf, 0, sizeof(buf));

	int msgLen = message.size();
	if (msgLen > DEFAULT_BUFLEN - sizeof(int)) {
		printf("Error encoding Message!");
		return buf;
	}
	int len = htonl(msgLen);
	
	memcpy(buf, &len, sizeof(int));
	memcpy(buf + sizeof(int), message.c_str(), message.size());
	std::cout << "buf: " << (int) buf << std::endl;
	return buf;
}