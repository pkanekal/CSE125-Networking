//#define __WINDOWS
#define __LINUX 

#ifdef __WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#endif

#ifdef __LINUX
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

#include "Client.h"
#define WIN32_LEAN_AND_MEAN

#include <stdlib.h>
#include <stdio.h>
#include <iostream>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#ifdef __WINDOWS
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#endif

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "5000"

Client::Client() : ConnectionEstablished(false), ConnectSocket(INVALID_SOCKET)
{
}

Client::Client(std::string ip, std::string port) {
	this->ConnectionEstablished = false;
	this->ConnectSocket = INVALID_SOCKET;
	this->serverIp = ip;
	this->port = port;
	int result = SetupTCPConnection(ip, port);
	if (result == -1) return;
}

Client::~Client()
{
	#ifdef __WINDOWS
		int iResult;
		iResult = shutdown(ConnectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			std::cerr << "shutdown failed: " << WSAGetLastError() << std::endl;
		}
		closesocket(ConnectSocket);

		WSACleanup();
	#endif
	this->ConnectionEstablished = false;
	this->ConnectSocket = INVALID_SOCKET;
}

int Client::CloseConnection(){
	int iResult;
	int closeError = 0;
	//Need to Establish Connection
	if (!ConnectionEstablished){
		std::cerr << "Already Closed" << std::endl;
		return 1;
	}
	#ifdef __WINDOWS
		if (iResult == SOCKET_ERROR) {
			std::cerr << "shutdown failed: " << WSAGetLastError() << std::endl;
			closeError = 1;
		}
		closesocket(ConnectSocket);
	#endif
	this->ConnectionEstablished = false;
	this->ConnectSocket = INVALID_SOCKET;
	return closeError;
}

int Client::SetupTCPConnection(std::string serverIp, std::string port){

	int ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	int iResult;

	// Initialize Winsock
	#ifdef __WINDOWS
		WSADATA wsaData;
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0) {
			printf("WSAStartup failed with error: %d\n", iResult);
			return -1;
		}

		ZeroMemory(&hints, sizeof(hints));
	#endif
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(serverIp.c_str(), port.c_str(), &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		#ifdef __WINDOWS
			WSACleanup();
		#endif
		return -1;
	}

	// Attempt to connect to an address until one succeeds TODO: Handle Timeouts?
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			#ifdef __WINDOWS
				printf("socket failed with error: %ld\n", WSAGetLastError());
				WSACleanup();
			#endif
			return -1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			#ifdef __WINDOWS
				closesocket(ConnectSocket);
			#endif
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	freeaddrinfo(result);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		#ifdef __WINDOWS
			WSACleanup();
		#endif
		return -1;
	}

	//Saving
	this->ConnectionEstablished = true;
	this->serverIp = serverIp;
	this->port = port;
	this->ConnectSocket = ConnectSocket;

	return 0;
}

int Client::sendMessage(std::string message){
	int iResult;

	//Need to Establish Connection
	if (!ConnectionEstablished){
		std::cerr << "Send Refused. Please Establish Connection" << std::endl;
		return 1;
	}

	iResult = send(this->ConnectSocket, message.c_str(), message.size(), 0);
	if (iResult == SOCKET_ERROR) {
		#ifdef __WINDOWS
			std::cerr << "Send Failed with error: " << WSAGetLastError() << std::endl;
			closesocket(ConnectSocket);
			WSACleanup();
		#endif
		this->ConnectionEstablished = false;
		this->ConnectSocket = INVALID_SOCKET;
		return 1;
	}

	//Pound Define This
	printf("Bytes Sent: %d\n", iResult);

	return 0;
}

std::string Client::receiveMessage(){
	int iResult;
	//CHANGE THE DEFAULT_BUFLEN TO DESIRE
	int buflen = DEFAULT_BUFLEN;
	char recvbuffer[DEFAULT_BUFLEN];
	memset(recvbuffer, 0, DEFAULT_BUFLEN);

	//Need to Establish Connection
	std::cout << "reached " << std::endl;
	if (!ConnectionEstablished){
		std::cerr << "Recv Refused. Please Establish Connection" << std::endl;
		return std::string();
	}
	ssize_t totalrecv = 0;
	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	setsockopt(ConnectSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));
	std::string result = "";

	do {
		if (totalrecv > buflen){
			std::cerr << "Buffer Overflow!!!!!" << std::endl;
			return std::string();
		}
		std::cout << "Receiving..." << std::endl;
		iResult = recv(ConnectSocket, recvbuffer, DEFAULT_BUFLEN-1, 0);
		std::cout << "Done!  iResult is " << iResult << std::endl;
		if (iResult > 0){
			totalrecv += iResult;
			recvbuffer[iResult] = '\0';
			result += recvbuffer;
		}
		else if (iResult == 0){
			std::cerr << "Connection Closed." << std::endl;
		}
		else{
			//Shutdown the connections
			this->ConnectionEstablished = false;
			this->ConnectSocket = INVALID_SOCKET;
			//DO we want to return a blank one or a partial one?
			if (errno == EWOULDBLOCK) {
				std::cerr << "errno is " << EWOULDBLOCK << std::endl;
			}
			#ifdef __WINDOWS
				std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
			#endif
		}
	} while (iResult > 0);

	//TODO:: IS THIS ACCURATE?
	return result;
}
