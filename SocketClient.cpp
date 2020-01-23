#include "SocketClient.h"
namespace GT {
	SocketClient::SocketClient(CSInfo pInfo) :info(pInfo) {

	}
	bool SocketClient::start() {
		// Initialize Winsock
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsa);
		if (iResult != 0) {
			printf("WSAStartup failed with error: %d\n", iResult);
			msgError = "WSAStartup failed with error";
			sockError = iResult;
			return false;
		}
		
		// Create socket
		struct addrinfo* result = NULL;
		struct addrinfo* ptr = NULL;
		struct addrinfo hints;

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		// Resolve the server address and port
		iResult = getaddrinfo(info.host, info.port, &hints, &result);
		if (iResult != 0) {
			msgError = "getaddrinfo failed with error";
			sockError = iResult;
			//printf("getaddrinfo failed with error: %d\n", iResult);
			WSACleanup();
			return false;
		}

		// Attempt to connect to an address until one succeeds
		for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

			// Create a SOCKET for connecting to server
			master = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
			if (master == INVALID_SOCKET) {
				msgError = "socket failed with error";
				sockError = WSAGetLastError();
				//printf("CLIENTE socket failed with error: %ld\n", WSAGetLastError());
				WSACleanup();
				return false;
			}


			// Connect to server.
			iResult = connect(master, ptr->ai_addr, (int)ptr->ai_addrlen);
			if (iResult == SOCKET_ERROR) {
				closesocket(master);
				master = INVALID_SOCKET;
				continue;
			}
			break;
		}

		freeaddrinfo(result);

		if (master == INVALID_SOCKET) {
			printf("Unable to connect to server!\n");
			WSACleanup();
			return false;
		}
		onConect();
		// listen
		char recvbuf[100];
		int recvbuflen = 100;
		
		do {

			puts(".");
			memset(&recvbuf, 0, sizeof(recvbuf));//clear the buffer
			iResult = recv(master, recvbuf, recvbuflen, 0);

			puts(recvbuf);
			if (iResult > 0) {
				
				onReceive(recvbuf, recvbuflen);
			} else if (iResult == 0) {
				printf("Connection closed\n");

			} else {
				printf("recv failed with error: %d\n", WSAGetLastError());
			}
				

		} while (iResult > 0);
		printf("BYEEEEE\n");
		return true;
	}
	void SocketClient::stop() {
	}
	void SocketClient::onReceive(char* buffer, size_t size) {
		printf("Message: %s\nBytes received: %d\n> ", buffer, size);
	}
	SOCKET SocketClient::getHost() {
		return master;
	}
}
