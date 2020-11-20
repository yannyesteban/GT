#pragma once

#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip> // para la fecha
#include "Color.h"
#include "Types.h"
#include "SocketClient.h"
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

		bool beginTasks();

	private:
		float syncTime = 10;
		float trackingTime = 5;

		float trackingDelay = 0;
		float syncDelay = 0;

		std::string unitName = "3024000100";

		void getDateTime(std::string &);
		std::string getDateTime();

		float getRand();
		float getLng();
		float getLat();

		time_t now = 0;

	};
}
