#include "Client.h"
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

Client::Client() : ConnectionEstablished(false), ConnectSocket(INVALID_SOCKET)
{
}


Client::~Client()
{
	int iResult;
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		std::cerr << "shutdown failed: " << WSAGetLastError() << std::endl;
	}
	closesocket(ConnectSocket);
	this->ConnectionEstablished = false;
	this->ConnectSocket = INVALID_SOCKET;
	WSACleanup();
}

int Client::CloseConnection(){
	int iResult;
	int closeerror = 0;
	//Need to Establish Connection
	if (!ConnectionEstablished){
		std::cerr << "Already Closed" << std::endl;
		return 1;
	}
	if (iResult == SOCKET_ERROR) {
		std::cerr << "shutdown failed: " << WSAGetLastError() << std::endl;
		closeerror = 1;
	}
	closesocket(ConnectSocket);
	this->ConnectionEstablished = false;
	this->ConnectSocket = INVALID_SOCKET;
	return closeerror;
}

int Client::SetupTCPConnection(std::string serverip, std::string port){
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(serverip.c_str(), port.c_str(), &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Attempt to connect to an address until one succeeds TODO: Handle Timeouts?
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	freeaddrinfo(result);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	//Saving
	this->ConnectionEstablished = true;
	this->serverip = serverip;
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
		std::cerr << "Send Failed with error: " << WSAGetLastError() << std::endl;
		closesocket(ConnectSocket);
		WSACleanup();
		this->ConnectionEstablished = false;
		this->ConnectSocket = INVALID_SOCKET;
		return 1;
	}

	//Pound Define This
	printf("Bytes Sent: %ld\n", iResult);

	return 0;
}

std::string Client::receiveMessage(){
	int iResult;
	//CHANGE THE DEFAULT_BUFLEN TO DESIRE
	int buflen = DEFAULT_BUFLEN;
	char recvbuffer[DEFAULT_BUFLEN];
	memset(recvbuffer, 0, DEFAULT_BUFLEN);

	//Need to Establish Connection
	if (!ConnectionEstablished){
		std::cerr << "Recv Refused. Please Establish Connection" << std::endl;
		return 1;
	}
	SSIZE_T totalrecv = 0;
	do {
		if (totalrecv > buflen){
			std::cerr << "Buffer Overflow!!!!!" << std::endl;
			return std::string();
		}
		iResult = recv(ConnectSocket, recvbuffer + totalrecv, buflen - totalrecv, 0);
		if (iResult > 0){
			totalrecv += iResult;
		}
		else if (iResult == 0){
			std::cerr << "Connection Closed." << std::endl;
		}
		else{
			//Shutdown the connections
			this->ConnectionEstablished = false;
			this->ConnectSocket = INVALID_SOCKET;
			//DO we want to return a blank one or a partial one?
			std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
			return std::string();
		}
	} while (iResult > 0);

	//TODO:: IS THIS ACCURATE?
	return std::string(recvbuffer, totalrecv);
}


int __cdecl main(int argc, char **argv)
{
	//std::cout << "hello" << std::endl;

	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	char *sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Validate the parameters
	if (argc != 2) {
		printf("usage: %s server-name\n", argv[0]);
		return 1;
	}

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	// Send an initial buffer
	iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	printf("Bytes Sent: %ld\n", iResult);

	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// Receive until the peer closes the connection
	do {

		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
			printf("Bytes received: %d\n", iResult);
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());

	} while (iResult > 0);

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();
	getchar();
	return 0;
}
