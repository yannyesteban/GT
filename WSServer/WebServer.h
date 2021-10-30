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
#include <thread>         // std::thread
#include <chrono>         // std::chrono::seconds
#include <iomanip> // para la fecha

struct HubConfig {
	const char* name;
	const char* host;
	unsigned int port;

};

rapidjson::Document loadConfig(const char* path);

void runHub(HubConfig* con, LPVOID param);

BOOL WINAPI mainhub(LPVOID param);
using namespace rapidjson;
namespace GT {
	void runTimer();

	

	struct WebClient {
		int id = -2;
		int version_id = -2;
		char user[40];
		char name[40];
		
		short int status = 0;
		SOCKET socket;
		short int type = 0;

	};

	HubConfig getHugConfig(rapidjson::Value& v);

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

		std::map<std::string, Hub*> hubs;
		int nHubs = 0;

		void sendCommand(int unitId, int commandId, int index, int mode);
		void sendToDevice(ConnInfo Info, int unitId, int commandId, int index, int mode, std::string user);
		void sendToDevice(SOCKET server, GT::RCommand * request);

		int sendToServers(char * buffer, int len);


		

	private:
		DB* db;
		
		HANDLE hClientThread;
		DWORD dwThreadId;
		std::map<SOCKET, WebClient> clients;

		std::string loadCommand(int unitId, int commandId, int index, int mode, std::string& role);
		bool insertEvent(DBEvent* infoEvent);
		
	};
}


