#include "JsonConfig.h"
using namespace rapidjson;
namespace GT {
	WSAppConfig JsonConfig::info = {};// { 0, 0, { "","","",0 } };
	Document JsonConfig::d = NULL;
    WSAppConfig JsonConfig::load(const char* path) {
		//"C:\\source\\cpp\\XT\\XTServer\\config.json"
		FILE* fp = fopen(path, "rb"); // non-Windows use "r"

		if (fp == NULL)	{
			perror("Error while opening the file.\n");
			exit(EXIT_FAILURE);
		}

		char* readBuffer;

		readBuffer = (char*)malloc(1500);
		FileReadStream is(fp, readBuffer, sizeof(readBuffer));
		//Document d;

		d.ParseStream(is);
		fclose(fp);

		//strcpy_s(info.appname, sizeof(info.appname), d["appname"].GetString());
		info.appname = d["appname"].GetString();
		
		info.db.host = d["db"]["host"].GetString();
		info.db.port = d["db"]["port"].GetString();
		info.db.user = d["db"]["user"].GetString();
		info.db.pass = d["db"]["pass"].GetString();
		info.db.name = d["db"]["name"].GetString();

		info.hub.host = d["hub"]["host"].GetString();
		info.hub.port = d["hub"]["port"].GetInt();
		
		info.debug = d["debug"].GetBool();
		info.max_clients = d["max_clients"].GetInt();
		
		info.port = d["port"].GetInt();
		info.show_cache = d["show_cache"].GetBool();
		info.version = d["version"].GetString();
		info.name = d["name"].GetString();
		

		/*
		strcpy_s(info.version, sizeof(info.version), d["version"].GetString());

		strcpy_s(info.db.dbname, sizeof(info.db.dbname), d["db"]["dbname"].GetString());
		strcpy_s(info.db.host, sizeof(info.db.host), d["db"]["host"].GetString());
		strcpy_s(info.db.user, sizeof(info.db.user), d["db"]["user"].GetString());
		strcpy_s(info.db.pass, sizeof(info.db.pass), d["db"]["pass"].GetString());
		strcpy_s(info.db.port, sizeof(info.db.port), d["db"]["port"].GetString());
		*/
		if (readBuffer != NULL) {
			readBuffer[0] = '\0';
			free(readBuffer);
		}

		return info;
    }
}