#include "DeviceAdmin.h"

std::mutex m2;

GT::DeviceAdmin::DeviceAdmin() {
}

void GT::DeviceAdmin::run(AppConfig * config) {

	auto db = new GT::DB2(config->db);
	db->connect();
	//db->init();

	
	db->loadUnits(&Devices);
	
	//db->loadTracking(2002, 11236);
	//return;
	//std::mutex m2;
	for (int i = 0; i < Devices.size(); i++) {
		std::cout <<"Device Id: " << Devices[i] << std::endl;
		//std::thread Tasks(runDevice);
		std::thread * first = new std::thread(runDevice, Devices[i], config, i, db);
		Tasks.push_back(first);
		//Sleep(1000);
	}
	std::cout << "\n\n\n\nEND\n\n\n";
	for (int i = 0; i < Devices.size(); i++) {
		std::cout << "\n Task Join "<< i << std::endl;
		Tasks[i]->join();
		/*
		std::thread first(runDevice);
		Tasks.push_back(first);
		*/
	}
	//std::thread first(runDevice);

	//irst.join();
}

void runDevice(int unitId, GT::AppConfig* config, int clientId, GT::DB2* db) {


	//std::lock_guard<std::mutex> guard(m);
	//std::unique_lock<std::mutex> ul(m);

	GT::CSInfo info;
	int band = 0;
	if (band >= 1) {
		info.host = (char*)"bests.no-ip.info";//"127.0.0.1";//
	} else {
		info.host = (char*)"localhost";//
	}
	
	std::cout << "\n*********\n Unit Id "<< unitId <<"\n\n";
	
	info.port = 3317;
	//std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	//std::this_thread::sleep_for(std::chrono::seconds(1));
	
	
	auto C = new GT::Device(info, db, m2);
	//C->m = m2;
	//C->db = db;
	C->clientId = clientId;
	C->init(config);
	//C->config = config;
	C->setUnitName(unitId);
	
	C->start();
	
	
	
}
