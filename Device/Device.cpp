#include "Device.h"

GT::Device::Device(CSInfo pInfo) :SocketClient(pInfo) {
	
}

void GT::Device::onConect() {
	
	syncTask();

	Sleep(100);
	

	
	time(&now);  /* get current time; same as: now = time(NULL)  */

	beginTasks();
}

bool GT::Device::isSyncMsg(char* buffer) {


	SyncMsg* syncMsg = (SyncMsg*)buffer;

	if (syncMsg->Keep_Alive_Header == 63738) {
		return true;
	}
	return false;

}

void GT::Device::onReceive(char* buffer, size_t size) {

	if (isSyncMsg(buffer)) {
		std::cout << "OK conectado correctamente" << std::endl;
	}
}

void GT::Device::syncTask() {
	SyncMsg s = { 63738,999,3024000100 };
	std::cout << "sincronizacion " << std::endl;

	char buffer[100];
	memcpy(buffer, &s, sizeof(s));

	send(getHost(), buffer, sizeof(s), 0);
}

void GT::Device::trackingTask() {

	std::string track = unitName + "," + getDateTime() + "," +
		std::to_string(getLng()) + "," + std::to_string(getLat()) + ",99,10,0,8,5,100,10,0,0,0,0,0,0,1,1,1,1";
	//system("cls");

	//const char* track = "3024000100,20201119052600,66.84869800,10.50286500,99,10,0,8,5,100,10,0,0,0,0,0,0";





	send(getHost(), track.c_str(), track.size(), 0);

}

bool GT::Device::beginTasks() {
	time_t time2;

	double elapsed;


	while (true) {
		Sleep(10);
		time(&time2);
		elapsed = difftime(time2, now);

		std::cout << elapsed << std::endl;
		if (trackingDelay >= trackingTime) {
			trackingTask();
			trackingDelay = 0;
		}

		if (syncDelay >= syncTime) {
			syncTask();
			syncDelay = 0;
		}

		trackingDelay += elapsed;
		syncDelay += elapsed;
		now = time2;

	}

	return false;
}

void GT::Device::getDateTime(std::string & datetime) {
	
	auto t = std::time(nullptr);
	//auto tm = *std::localtime_s(&t,);
	struct tm timeinfo;
	time_t rawtime;
	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);
	//std::cout << std::put_time(&timeinfo, "%F %T") << std::endl;

	std::stringstream wss;
	wss << std::put_time(&timeinfo, "%F %T");

	datetime = wss.str();
}

std::string GT::Device::getDateTime() {
	std::string datetime;
	getDateTime(datetime);
	return datetime;
}

float GT::Device::getRand() {


	srand((unsigned int)time(NULL));
	
	float a = 1.0;
	
	return (float(rand()) / float((RAND_MAX)) * a);
}

float GT::Device::getLng() {
	return -66.92106500 + getRand() / 10 ;
}

float GT::Device::getLat() {
	return 10.49768800 + getRand() / 10;
}
