#pragma once
#include <string>

namespace GT {

	enum class ClientMsg {
		Nothing,
		Connecting,
		Disconnecting,
		Message,
		CommandResponse,
		CommandError,
		Error,
		Request


	};
	
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
		int keep_alive;
		InfoDB db;
	};

	typedef struct {
		unsigned short Keep_Alive_Header;
		unsigned short Keep_Alive_ID;
		unsigned long Keep_Alive_Device_ID;
	} SyncMsg;

	struct ClientInfo{
		unsigned int unitId;
		unsigned char status;
		
	};

	struct Versions {
		int e[50];
		int n = 0;
	};

	typedef struct {
		//unsigned int type;
		char token;
		char id[11];
		//char type;
		char message[255];
		
		
	} Command;

	typedef struct {
		char id[11];
		char message[255];
	} DeviceMSG;

	struct IdHeader {
		unsigned short header;//10010
		unsigned short type;
	};

	struct RequestConnection {
		unsigned short header;//10001
		unsigned int socket;
		char name[41];
		char user[41];
		unsigned short type = 0;
		int version_id = -2;
		char message[255];
		short int status = 0;
		
		

	};

	struct DeviceMsg {
		unsigned short header;//10010
		unsigned short type;
		unsigned short device_id;
		char device_name[11];
		
		char msg[255];
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
		char params[255];
		unsigned short level;
		unsigned short mode;
		
		
	};


	struct InfoClient {

		int unit_id;
		int device_id;
		int version_id;
		int format_id;
		char name[41] = "";
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
		unsigned int id;
		char user[41] = "";
		char message[255] = "";
		char unit[11] = "";
		char name[41] = "";
		int unitId;
		int commandId;
		int mode;
		char date[20] = "";
		
		unsigned short level;
		unsigned short index;
		ClientMsg typeMessage = ClientMsg::Nothing;
		time_t time;
		double delay = 0;
		unsigned short commandIndex = 0;
	};

	struct PendingCommand {
		std::string name;
		std::string command;
	};
	
	
}

