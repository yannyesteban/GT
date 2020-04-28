#pragma once

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filereadstream.h"
#include "Types.h"
#include "WebSocketServer.h"
#include "Hub.h"
#include "Config.h"
#include "DB.h"
BOOL WINAPI mainhub(LPVOID param);

namespace GT {
	class WebServer : public WebSocketServer {
	public:
		WebServer(SocketInfo pInfo);
		void init();
		void onMessage(ConnInfo Info);
		void send2(char* buffer);
		void onConnect(ConnInfo Info);

		Hub * hub;
		int Token = 4737;
	private:
		DB* db;
		
		HANDLE hClientThread;
		DWORD dwThreadId;
	};
}


