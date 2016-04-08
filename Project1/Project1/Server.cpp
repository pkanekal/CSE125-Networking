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

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include "server.h"
#include <iostream>

// Need to link with Ws2_32.lib
#ifdef __WINDOWS
#pragma comment (lib, "Ws2_32.lib")
#endif
// #pragma comment (lib, "Mswsock.lib")

Server::Server(std::string port) {
	this->port = port;
	std::cout << "Starting server on port " << port << std::endl;
	listenSocket = setupSocket(port);
	
	if (listenSocket == -1) {
		printf("Error in setupSocket()");
	}
}


Server::~Server() {
	// shutdown the connection since we're done
	std::cout << "calling destructor" << std::endl;
	#ifdef __WINDOWS
		int iResult = shutdown(clientSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			printf("shutdown failed with error: %d\n", WSAGetLastError());
			closesocket(clientSocket);
			WSACleanup();
			return;
		}

		// cleanup
		closesocket(clientSocket);
		WSACleanup();
	#endif
}

void Server::start() {
	while (true) {
		clientSocket = acceptTCPConnection(listenSocket);
		handleClient(clientSocket);

		//	break;
	}
}

void Server::handleClient(int clientSocket) {
		// Receive until the peer shuts down the connection
	int iResult;
	std::string data;
	do {
		char recvbuf[DEFAULT_BUFLEN];
		iResult = recv(clientSocket, recvbuf, DEFAULT_BUFLEN-1, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
			recvbuf[iResult] = '\0';
			data += recvbuf;
			std::cout << "Received the following data: " << data << std::endl;

			// Echo the buffer back to the sender
			int iSendResult = send(clientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				#ifdef __WINDOWS
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(clientSocket);
					WSACleanup();
				#endif
				return;
			}
			printf("Bytes sent: %d\n", iSendResult);
			#ifdef __LINUX
				close(clientSocket);
			#endif
			return;
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else  {
			#ifdef __WINDOWS
				printf("recv failed with error: %d\n", WSAGetLastError());
				closesocket(clientSocket);
				WSACleanup();
			#endif
			return;
		}

	} while (iResult > 0);
}

int Server::acceptTCPConnection(int listenSocket) {
	// Accept a client socket
	int clientSocket;
	clientSocket = accept(listenSocket, NULL, NULL);

	if (clientSocket == INVALID_SOCKET) {
		#ifdef __WINDOWS
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(listenSocket);
			WSACleanup();
		#endif
		return -1;
	}

	// No longer need server socket
	#ifdef __WINDOWS
		closesocket(listenSocket);
	#endif

	return clientSocket;
}

int Server::setupSocket(std::string port) {

	#ifdef __WINDOWS
		WSADATA wsaData;
	#endif

	int iResult;

	int ListenSocket = INVALID_SOCKET;
	int ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	#ifdef __WINDOWS
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	#endif

	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return -1;
	}
	#ifdef __WINDOWS
		ZeroMemory(&hints, sizeof(hints));
	#endif

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, port.c_str(), &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		#ifdef __WINDOWS
			WSACleanup();
		#endif
		return -1;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		#ifdef __WINDOWS
			printf("socket failed with error: %ld\n", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
		#endif
		return -1;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		#ifdef __WINDOWS
			printf("bind failed with error: %d\n", WSAGetLastError());
			freeaddrinfo(result);
			closesocket(ListenSocket);
			WSACleanup();
		#endif
		return -1;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		#ifdef __WINDOWS
			printf("listen failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
		#endif
		return -1;
	}
	return ListenSocket;
}
