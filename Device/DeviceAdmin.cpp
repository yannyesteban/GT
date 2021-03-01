#include "DeviceAdmin.h"

GT::DeviceAdmin::DeviceAdmin() {
	//Devices.push_back("3024000100");
	//Devices.push_back("3034000101");
	Devices.push_back("2012000398");
	Devices.push_back("2012000742");
	Devices.push_back("2012000733");
	Devices.push_back("2012000731");
	Devices.push_back("2012000705");
	Devices.push_back("2012000715");
	Devices.push_back("2012000727");
	Devices.push_back("2012000683");
	Devices.push_back("2012000740");
	Devices.push_back("2012000728");
	Devices.push_back("2012000709");
	Devices.push_back("2012000708");

	Devices.push_back("2012000133");
	Devices.push_back("2012000134");
	Devices.push_back("2012000158");
	Devices.push_back("2012000278");
	Devices.push_back("2012000265");
	Devices.push_back("2012000262");
	Devices.push_back("2012000264");
	Devices.push_back("2012000256");
	Devices.push_back("2012000243");
	Devices.push_back("2012000242");

	Devices.push_back("2012000767");
	Devices.push_back("2012000302");
	Devices.push_back("2012000292");
	Devices.push_back("2012000309");
	Devices.push_back("2012000311");
	Devices.push_back("2012000258");
	Devices.push_back("2012000289");
	Devices.push_back("2012000313");
	Devices.push_back("2012000277");
	Devices.push_back("2012000435");



}

void GT::DeviceAdmin::run() {

	for (int i = 0; i < nTask; i++) {
		//std::thread Tasks(runDevice);
		std::thread * first = new std::thread(runDevice, Devices[i]);
		Tasks.push_back(first);
		Sleep(10000);
	}
	for (int i = 0; i < nTask; i++) {
		Tasks[i]->join();
		/*
		std::thread first(runDevice);
		Tasks.push_back(first);
		*/
	}
	//std::thread first(runDevice);

	//irst.join();
}

void runDevice(std::string name) {
	GT::CSInfo info;
	info.host = (char*)"127.0.0.1";
	info.port = 3322;
	
	auto C = new GT::Device(info);
	C->setUnitName(name);
	C->start();
}
