#pragma once
#include <sstream>
#include <ctime>
#include <thread>         // std::thread
#include <mutex>
#include <chrono>         // std::chrono::seconds
#include <mysql/jdbc.h>
#include "Color.h"
#include "Types.h"
#include "Socket.h"
//#include "Tool.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filereadstream.h"
#include "Webcar.h"
#include <iomanip> // para la fecha
#include "ProtoDB.h"

#include "Cmd.h"

namespace GT {
	struct GTClient1 {
		SOCKET socket;
		short int type = 0;
		int id = -2;
		std::string name;
		std::string address;
		int verionId = -2;

		clock_t clock;
		clock_t lastClock;

		std::string user;

		short int status = 0;

	};

	struct RClient {
		SOCKET socket;
		char name[41];
		char user[41];
		short int type = 0;
		int version_id = -2;
		short int status = 0;
		

	};

	struct GTClient {
		int id = -2;
		int version_id = -2;

		char device_id[10];
		short int status = 0;
		SOCKET socket;
		short int type = 0;
		clock_t clock;
		clock_t lastClock;

		unsigned short header;

		char name[40];
		char address[20];
		

	};

	struct GTClient2 {
		SOCKET socket;
		short int type = 0;
		int id = -2;
		char name[50];
		char address[20];
		int formatId = -2;
		
		clock_t clock;
		clock_t lastClock;
		
		char user[50];
		
		short int status = 0;

	};

	

	
	
	class Server: public Socket {
	public:
		Server(SocketInfo pInfo);
		~Server();
		ProtoDB* db;

		bool init(AppConfig pConfig);
		void onConnect(ConnInfo Info);
		void onMessage(ConnInfo Info);
		void onClose(ConnInfo Info);

		
		bool isSyncMsg(ConnInfo Info);
		bool isSyncMsg2(ConnInfo Info);
		unsigned short getHeader(ConnInfo Info);

		IdHeader * getMsgHeader(const char * msg);
		

		bool evalMessage(ConnInfo Info, const char* msg);
		bool deviceMessage(ConnInfo Info);
		bool deviceMessage2(ConnInfo Info);

		void broadcast(RCommand * response);
		void isAlive();
		void closeClient(SOCKET client);
		void deleteClient(SOCKET client);
		int keepAliveTime = 90;
		int waitTime = 30;
		
		Cmd cmd;
	private:
		bool debug;

		std::vector<int> synchValue;
		std::vector<std::string> identExp;
		std::map<int, ClientProto> mProto;
		
		clock_t mClock;
		//std::map<SOCKET, RClient> rClients;
		std::map<SOCKET, GTClient1> clients;
		//std::map<string, GTClient2 > mDevices;
		std::map<int, std::string> mUnitName;
		std::map<int, std::string> mClientName;
		//AppConfig * config;
		//Versions versions;
		//sql::Driver* driver = nullptr;
		//sql::Connection* cn = nullptr;
		//sql::Statement* stmt = nullptr;
		//sql::ResultSet* result = nullptr;
		std::string getUnitName(int unitId);
		void setUnitName(int unitId, std::string name);

		std::string getClientName(int unitId);
		void setClientName(int unitId, std::string name);

		WC::Webcar * webcar;
		SOCKET getSocket(int id);
		SOCKET getSocket(std::string name);

		bool insertEvent(InfoEvent & infoEvent);
		bool setClientStatus(unsigned int unitId, unsigned int status);
		bool saveTrack(int unitId, int formatId, const char* buffer);
		bool isVersion(int value);
		InfoClient getInfoClient(std::string id);
		int updateCommand(int unitId, int commandId, int index, int mode, std::string params);
		void getIndexCommand(const char* unit_id, CommandResult* commandResult, RCommand* info);
		void infoCommand(const char* unit_id, CommandResult* commandResult, RCommand* info);
		void insertTrack(std::string name, std::string track);
		//RCommand unitResponse;


		void defineClient(ConnInfo Info, std::string name, int type, std::string user);
		
		bool isTracking(ConnInfo Info);
		bool isCommand(ConnInfo Info);
		std::string isSynch(ConnInfo Info);
		std::string isDevice(ConnInfo Info);

		unsigned short evalRequest(ConnInfo Info);

		bool sendCommand(int id, std::string message);

		void wcTracking(int unitId, std::map<std::string, std::string>& data);

		int returnMessage(RCommand& unitResponse);
	};

}