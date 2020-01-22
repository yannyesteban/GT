#pragma once
#include <sstream>
#include <mysql/jdbc.h>
#include "Types.h"
#include "Socket.h"
#include "DB.h"



namespace GT {

	struct GTClient {
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

		bool evalMessage(const char* msg);


	private:
		bool debug;
		DB * db;

		std::map<SOCKET, GTClient> clients;
		//AppConfig * config;
		//Versions versions;
		//sql::Driver* driver = nullptr;
		//sql::Connection* cn = nullptr;
		//sql::Statement* stmt = nullptr;
		//sql::ResultSet* result = nullptr;
	};

}