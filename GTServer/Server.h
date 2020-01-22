#pragma once

#include <mysql/jdbc.h>
#include "Types.h"
#include "Socket.h"
#include "DB.h"



namespace GT {



	class Server: public Socket {
	public:
		Server(SocketInfo pInfo);
		bool init(AppConfig pConfig);
		void onConnect(ConnInfo Info);
		void onMessage(ConnInfo Info);
		void onClose(ConnInfo Info);

		int connectDB(InfoDB pInfo);
		bool isSyncMsg(const char * buffer, char* id);


	private:
		bool debug;
		DB * db;
		AppConfig * config;
		Versions versions;
		sql::Driver* driver = nullptr;
		sql::Connection* cn = nullptr;
		sql::Statement* stmt = nullptr;
		sql::ResultSet* result = nullptr;
	};

}