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
//#include "DB.h"
#include "ProtoDB.h"
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
	
	struct ConnectInfo {
		std::string name;
		std::string user;
	};

	struct CommandInfo {
		int unitId;
		std::string command;
		int index;
		int mode;
		int status;
		std::string name;
		std::string params;
		std::string query;
		std::string values;
		std::string user;
	};
	

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

		void sendCommand(GT::RCommand);

		int sendToServers(char * buffer, int len);


		ConnectInfo getConnectInfo(const rapidjson::Document& jsonDocument);
		CommandInfo getCommandInfo(const rapidjson::Document& jsonDocument);
		void startClient(ConnectInfo);
		void evalCommand(CommandInfo);
	private:
		ProtoDB* db;
		
		HANDLE hClientThread;
		DWORD dwThreadId;
		std::map<SOCKET, WebClient> clients;

		std::string loadCommand(int unitId, int commandId, int index, int mode, std::string& role, int & roleId);
		bool insertEvent(DBEvent* infoEvent);
		
	};
}


