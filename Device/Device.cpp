#include "Device.h"

GT::Device::Device(CSInfo pInfo, DB2* db2, std::mutex& mu) :mMutex(mu), SocketClient(pInfo) {
	
	db = db2;
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

	std::cout << "\nUNIT NAME " << unitName << "\n";

	SyncMsg s = { 63738,999,std::stoll(unitName) };
	std::cout << "SYNCH " << std::endl;

	char buffer[100];
	memcpy(buffer, &s, sizeof(s));

	isValid();

	send(getHost(), buffer, sizeof(s), 0);
}

void GT::Device::trackingTask() {
	//std::lock_guard<std::mutex> guard(mMutex);
	mMutex.lock();
	std::cout << " BEGIN ID : " << beginId << "\n\n";
	std::string cmd = db->loadTracking(unitId, &beginId, format);

	//cmd = "&&G142,5024000001,000,0,,211124115656,V,10.502798,-66.848701,0,0.0,0,0,0,0,734|2|02CE|002A2101,21,00000038,02,00,04B6|01A2|0000|0000,1,020000,,7D";

	mMutex.unlock();
	std::cout << "Send: " << "Clien ID: " << clientId << ", " << std::endl;
	send(getHost(), cmd.c_str(), cmd.size(), 0);

}

void GT::Device::init(AppConfig* config) {
	//db = new GT::DB2(config->db);
	db->connect();
	//db->init();

}

bool GT::Device::beginTasks() {
	time_t time2;
	timespec ts2;
	float elapsed;
	int base;
	Sleep(5000);
	int d = 0;
	while (true) {
		d++;
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
		
		if (clientId == 3 && d == 40) {
			std::cout << "STOP \n\n";
			stop();
		}

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

int GT::Device::isValid()
{
	int sockError = WSAGetLastError();
	printf("CLIENTE socket failed with error: %ld\n", WSAGetLastError());

	std::cout << "\nACTUAL SOKET " << getHost() << "\n";
	if (WSAGetLastError() > 0) {
		start();
		return 0;
	}
	return 1;
}
