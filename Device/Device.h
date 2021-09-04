#pragma once

#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip> // para la fecha
#include "Color.h"
#include "Types.h"
#include "SocketClient.h"
#include "DB2.h"

namespace GT {

	class Device :public SocketClient {
	public:
		Device(CSInfo pInfo);
		virtual void onConect();
		bool isSyncMsg(char * buffer);
		void onReceive(char* buffer, size_t size);
		void syncTask();
		void trackingTask();
		void commandTask();
		void evalCommand();
		void init(AppConfig* config);

		bool beginTasks();

		void setUnitName(int unitId);
		DB2 * db;
		AppConfig* config;
		int clientId = 0;

	private:
		float syncTime = 20;
		float trackingTime = 5;
		std::vector<std::string> format;
		float trackingDelay = 0;
		float syncDelay = 0;
		int beginId = 2560483;

		std::string unitName = "";
		int unitId = 0;

		void getDateTime(std::string &);
		std::string getDateTime();

		float getRand();
		float getLng();
		float getLat();

		int isValid();

		time_t now = 0;
		timespec ts;

	};
}
