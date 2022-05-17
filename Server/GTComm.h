#pragma once
#include <iostream>
#include "Server.h"
#include "Color.h"
#include "Config.h"
#include <iomanip> // para la fecha

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filereadstream.h"
#include <cstdio>
#include <iostream>

struct GTAppConfig {
	const char* appname;
	const char* name;
	unsigned int port;
	unsigned int max_clients;
	const char* version;
	bool debug;
	bool show_cache;
	int keep_alive;
	
};
rapidjson::Document loadConfig(const char* path);
GT::AppConfig getConfig(rapidjson::Value& v);


void runServer(GT::AppConfig* config);

namespace GT {

	class GTComm
	{
	public:
		GTComm(GTAppConfig pConfig);
		~GTComm();

		void start(std::string fileConfig);
	
	private:
		GTAppConfig config;

	};
}


