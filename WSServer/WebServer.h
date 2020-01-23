#pragma once
#include "WebSocketServer.h"
#include "Hub.h"
BOOL WINAPI mainhub(LPVOID param);
namespace GT {
	class WebServer : public WebSocketServer {
	public:
		WebServer(SocketInfo pInfo);
		void init();
		void onMessage(ConnInfo Info);
		Hub * hub;
	private:

		
		HANDLE hClientThread;
		DWORD dwThreadId;
	};
}


