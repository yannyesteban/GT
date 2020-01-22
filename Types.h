#pragma once

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
		char token;
		char id[10];
		char message[100];
	} Command;

	
}

