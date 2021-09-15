#pragma once
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filereadstream.h"
#include <cstdio>
#include <iostream>
#include "Types.h"

namespace GT {
	
	struct InfoHub {
		const char* host;
		unsigned int port;

	};

	struct WSAppConfig {
		const char* appname;
		
		InfoDB db;
		InfoHub hub;
		bool debug;
		unsigned int max_clients;
		unsigned int port;
		bool show_cache;
		const char* version;
		const char* name;
		
	};

	class JsonConfig {

	public:

		static rapidjson::Document d;
		static WSAppConfig load(const char* path);

		static WSAppConfig getInfo() {
			return info;
		}
	private:
		static WSAppConfig info;

	};

}
