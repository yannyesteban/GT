#pragma once
#include <string>
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/writer.h" // for stringify JSON
#include "rapidjson/filereadstream.h"
#include "Types.h"
#include "WebSocketServer.h"
#include "Hub.h"
#include "Config.h"
#include "JsonConfig.h"
#include "DB.h"
BOOL WINAPI mainhub(LPVOID param);

namespace GT {

	struct WebClient {
		int id = -2;
		int version_id = -2;

		char name[40];
		short int status = 0;
		SOCKET socket;
		short int type = 0;

	};

	class WebServer : public WebSocketServer {
	public:
		WebServer(SocketInfo pInfo);
		void init();
		void onMessage(ConnInfo Info);
		void send2(char* buffer);
		void onConnect(ConnInfo Info);
		void test2(unsigned int);
		WebClient getClient(void);
		unsigned short getHeader(char * Buffer);
		void jsonResponse(SOCKET client, RCommand * response);
		//AppConfig loadConfig(const char* path);
		Hub * hub;
		WSAppConfig configInit;
		int Token = 4737;
		bool reconnect = true;
	private:
		DB* db;
		
		HANDLE hClientThread;
		DWORD dwThreadId;
		std::map<SOCKET, WebClient> clients;
		
	};
}


