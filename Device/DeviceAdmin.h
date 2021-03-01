#pragma once

#include <iostream>       // std::cout
#include <thread>         // std::thread
#include "Device.h"
#include <vector>

void runDevice(std::string name);

namespace GT {
	class DeviceAdmin {
	public:
		DeviceAdmin();
		int nTask = 3;
		void run();
	private:
		std::vector<std::thread *> Tasks;
		std::vector<std::string> Devices;

	};
}

