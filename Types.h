#pragma once
#include <string>

namespace GT {

	
	struct InfoDB {

		const char* host;
		const char* port;
		const char* name;
		const char* user;
		const char* pass;
		bool debug;

	};
	struct AppConfig {
		const char* appname;
		unsigned int port;
		unsigned int max_clients;
		const char* version;
		bool debug;
		bool show_cache;
		InfoDB db;
	};

	typedef struct {
		unsigned short Keep_Alive_Header;
		unsigned short Keep_Alive_ID;
		unsigned long Keep_Alive_Device_ID;
	} SyncMsg;

	struct Versions {
		int e[50];
		int n = 0;
	};

	typedef struct {
		//unsigned int type;
		char token;
		char id[11];
		//char type;
		char message[120];
		
		
	} Command;

	typedef struct {
		char id[11];
		char message[100];
	} DeviceMSG;

	struct IdHeader {
		unsigned short header;//10010
		unsigned short type;
	};

	struct DeviceMsg {
		unsigned short header;//10010
		unsigned short type;
		unsigned short device_id;
		char device_name[11];
		
		char msg[100];
	};

	struct CMDMsg {
		unsigned short header;//10010
		unsigned short type;
		
		unsigned short unitId;
		unsigned short deviceId;
		char deviceName[11];
		unsigned short cmdId;//
		
		char tag[21];//10010
		char password[5];//10010
		char params[200];
		
		
	};


	struct InfoClient {

		int unit_id;
		int device_id;
		int version_id;
	};

	struct CommandResult {
		std::string token;
		std::string command;
		std::string tag;
		std::string params;

	};

	struct RCommand {
		unsigned short header;
		unsigned short type;
		char user[41];
		char message[101];
		char unit[11];
		int unitId;
		int mode;

	};
	
}

