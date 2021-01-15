#pragma once
#include <sstream>
#include <ctime>
#include <thread>         // std::thread
#include <chrono>         // std::chrono::seconds
#include <mysql/jdbc.h>
#include "Color.h"
#include "Types.h"
#include "Socket.h"
#include "DB.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filereadstream.h"
#include "Webcar.h"
#include <iomanip> // para la fecha
namespace GT {
	

	struct RClient {
		SOCKET socket;
		char name[41];
		char user[41];
		short int type = 0;
		int version_id = -2;
		short int status = 0;
		

	};

	struct GTClient {
		int id = -2;
		int version_id = -2;

		char device_id[10];
		short int status = 0;
		SOCKET socket;
		short int type = 0;
		

	};

	

	void runPending(std::map<string, GTClient > * mDevices, DB* db);
	void runTimer();
	class Server: public Socket {
	public:
		Server(SocketInfo pInfo);
		~Server();
		bool init(AppConfig pConfig);
		void onConnect(ConnInfo Info);
		void onMessage(ConnInfo Info);
		void onClose(ConnInfo Info);

		
		bool isSyncMsg(ConnInfo Info);
		unsigned short getHeader(ConnInfo Info);

		IdHeader * getMsgHeader(const char * msg);
		

		bool evalMessage(ConnInfo Info, const char* msg);
		bool deviceMessage(ConnInfo Info);

		void broadcast(RCommand * response);
	private:
		bool debug;
		DB * db;
		
		std::map<SOCKET, RClient> rClients;
		std::map<SOCKET, GTClient> clients;
		std::map<string, GTClient > mDevices;
		std::map<int, string> mUnitName;
		std::map<int, string> mClientName;
		//AppConfig * config;
		//Versions versions;
		//sql::Driver* driver = nullptr;
		//sql::Connection* cn = nullptr;
		//sql::Statement* stmt = nullptr;
		//sql::ResultSet* result = nullptr;
		std::string getUnitName(int unitId);
		void setUnitName(int unitId, std::string name);

		std::string getClientName(int unitId);
		void setClientName(int unitId, std::string name);

		WC::Webcar * webcar;
		//RCommand unitResponse;
	};

}