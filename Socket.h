#pragma once
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define FD_SETSIZE 2048
#include "ConsoleColors.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <windows.h>
#include <winsock2.h>
#include <ctime>

#include <iostream>
#include <stdio.h>

//#include "sha1.h"
#pragma comment(lib, "ws2_32.lib") //Winsock Library
namespace GT {

	typedef struct  ConnInfob{
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
		clock_t clock;
	} ConnInfo;

	typedef struct SocketInfob {
		char* host;
		u_short port;
		u_short maxClients;
	} SocketInfo;


	class Socket {
	public:
		Socket(SocketInfo pInfo);
		void start();
		void stop();

		int disconnect(SOCKET client);
	
		void (*CallConection)(ConnInfo Info);
		void (*CallMessage)(ConnInfo Info);
		void (*CallClose)(ConnInfo Info);
		
		virtual void onConnect(ConnInfo Info)=0;
		virtual void onMessage(ConnInfo Info)=0;
		virtual void onClose(ConnInfo Info)=0;

		void startListen();
		void listSocket();
	private:
		WSADATA wsa = {};
		SOCKET master = 0;
		SOCKET tSocket = 0;
		SOCKET clients[3000];
		

		sockaddr_in server;
		sockaddr_in address;

	protected:
		SocketInfo info = {};
		u_short maxClients = 200;

	};

}


