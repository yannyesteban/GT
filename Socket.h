#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "ConsoleColors.h"

#include<stdio.h>
#include <stdint.h>
#include<winsock2.h>
#pragma comment(lib, "ws2_32.lib") //Winsock Library
namespace GT {

	typedef struct  {
		SOCKET master;
		SOCKET client;
		SOCKET * clients;
		int index;
		u_short maxClients;
		int error = 0;
		char* buffer;
		int valread;

		char* address;
		u_short port;
		char* tag;
	} ConnInfo;

	typedef struct  {
		char* host;
		u_short port;
		u_short maxClients;
	} SocketInfo;
	class Socket {
	public:
		Socket(SocketInfo pInfo);
		void start();
		void stop();
	
		void (*CallConection)(ConnInfo Info);
		void (*CallMessage)(ConnInfo Info);
		void (*CallClose)(ConnInfo Info);
		
		void startListen();
	private:
		WSADATA wsa = {};
		SOCKET master = 0;
		SOCKET tSocket = 0;
		SOCKET clients[30];
		SocketInfo info = {};
		u_short maxClients = 100;

		sockaddr_in server;
		sockaddr_in address;

		

	};

}


