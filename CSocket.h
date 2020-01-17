#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
namespace GT {
	typedef struct {
		SOCKET master;
		SOCKET client;
		SOCKET* clients;
		int index;
		u_short maxClients;
		int error = 0;
		char* buffer;
		int valread;

		char* address;
		u_short port;
		char* tag;
		char* host;
		const char * port;
	} CSInfo;

	class CSocket {
	public:
		CSocket(CSInfo);
		bool start();
		void stop();

		virtual void onReceive(char* buffer, size_t size);



	private:
		CSInfo info;
		int sockError = 0;
		const char* msgError;
		WSADATA wsa = {};
		SOCKET master = 0;
	};

}