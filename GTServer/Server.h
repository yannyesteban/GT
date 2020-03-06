#pragma once
#include <sstream>
#include <mysql/jdbc.h>
#include "Types.h"
#include "Socket.h"
#include "DB.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filereadstream.h"


namespace GT {


	

	struct GTClient {
		int id = -2;
		int version_id = -2;

		char device_id[10];
		short int status = 0;
		SOCKET socket;
		short int type = 0;

	};

	class Server: public Socket {
	public:
		Server(SocketInfo pInfo);
		bool init(AppConfig pConfig);
		void onConnect(ConnInfo Info);
		void onMessage(ConnInfo Info);
		void onClose(ConnInfo Info);

		
		bool isSyncMsg(ConnInfo Info);
		unsigned short getHeader(ConnInfo Info);

		bool evalMessage(ConnInfo Info, const char* msg);
		bool deviceMessage(ConnInfo Info);


	private:
		bool debug;
		DB * db;

		std::map<SOCKET, GTClient> clients;
		std::map<string, GTClient > mDevices;
		//AppConfig * config;
		//Versions versions;
		//sql::Driver* driver = nullptr;
		//sql::Connection* cn = nullptr;
		//sql::Statement* stmt = nullptr;
		//sql::ResultSet* result = nullptr;
	};

}