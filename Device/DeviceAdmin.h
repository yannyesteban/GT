#pragma once

#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>
#include "Device.h"
#include <vector>
#include "DB2.h"
//#include "Types.h"
#include "GTServer/Config.h"

void runDevice(int unitId, GT::AppConfig * config, int clientId, GT::DB2 * db);

namespace GT {
	class DeviceAdmin {
	public:
		DeviceAdmin();
		int nTask = 1;
		void run(AppConfig * config);
		DB2 * db;
	private:
		std::vector<std::thread *> Tasks;
		std::vector<int> Devices;
		

	};
}

