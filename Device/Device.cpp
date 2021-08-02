#include "Device.h"

GT::Device::Device(CSInfo pInfo) :SocketClient(pInfo) {
	
}

void GT::Device::onConect() {
	
	syncTask();

	Sleep(500);
	

	
	time(&now);  /* get current time; same as: now = time(NULL)  */
	timespec_get(&ts, TIME_UTC);
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
	
	//std::cout << "................ HOLAAAAAAAA" << std::endl;

	if (isSyncMsg(buffer)) {
		std::cout << "OK conectado correctamente" << std::endl;
		return;
	} else {
		std::cout << "enviando Response !!!" << std::endl;
		//char buffer[100];
		const char *s = "$OK:VER+3=X0_C8_2.057XT_SG111";
		//memcpy(buffer, "$wp+ver+1=yanny", sizeof("$wp+ver+1=yanny"));
		send(getHost(), s, strlen(s), 0);
	}


}

void GT::Device::syncTask() {
	//SyncMsg s = { 63738,999,3024000100 };
	SyncMsg s = { 63738,999,std::stoll(unitName) };
	std::cout << "SYNCH " << std::endl;

	char buffer[100];
	memcpy(buffer, &s, sizeof(s));

	send(getHost(), buffer, sizeof(s), 0);
}

void GT::Device::trackingTask() {

	std::string cmd = db->loadTracking(unitId, &beginId, format);
	std::cout << "Send: " << "Clien ID: " << clientId << ", " << std::endl;
	send(getHost(), cmd.c_str(), cmd.size(), 0);

}

void GT::Device::init(AppConfig* config) {
	db = new GT::DB2(config->db);
	db->connect();
	//db->init();

}

bool GT::Device::beginTasks() {
	time_t time2;
	timespec ts2;
	float elapsed;
	int base;

	while (true) {
		
		Sleep(200);
		//time(&time2);
		base = timespec_get(&ts2, TIME_UTC);

		elapsed = (ts2.tv_sec + ts2.tv_nsec * 1e-9) - (ts.tv_sec + ts.tv_nsec * 1e-9);
		//elapsed = difftime(time2, now);

		
		if (trackingDelay >= trackingTime) {
			std::cout << unitName << " Tracking: " << elapsed << std::endl;
			trackingTask();
			trackingDelay = 0;
		}

		if (syncDelay >= syncTime) {
			std::cout << "Clien ID: " << clientId << ", " << unitName << " Sync: " << elapsed << std::endl;
			syncTask();
			syncDelay = 0;
		}

		trackingDelay += elapsed;
		syncDelay += elapsed;
		//now = time2;
		ts = ts2;

		//break;

	}

	return false;
}

void GT::Device::setUnitName(int id) {
	unitId = id;
	UnitInfo info;
	db->getInfoUnit(id, &info);
	unitName = info.unitName;
	std::cout << "Unit Id: " << unitId << " UnitName: " << info.unitName << std::endl;
	
	//db->loadFormat(unitId, &format);
	
	//
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
