#pragma once

#define MULTILINE(...) #__VA_ARGS__

#include "Types.h"
#include "Tool.h"
#include "ConsoleColors.h"
#include <mysql/jdbc.h>
#include<string>
//
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filereadstream.h"

namespace GT {
	struct InfoProto {
		
		int id_device;
		int tag_length;
		std::string pass_default;
		std::string protocol_pre;
		std::string sync_header;
		int format_id;

	};

	struct InfoClientX {
		
		int unit_id;
		int device_id;
		int version_id;
	};

	struct Format {
		char s[30][20];
		int n = 0;
	};

	struct InfoProto3 {

		int id_device;
		int tag_length;
		char* pass_default;
		char* protocol_pre;
		char* sync_header;

	};


	class DB {
	public:
		DB(InfoDB pInfo);
		bool connect();

		void loadProtocols();
		void printProtocols();

		void loadVersions();
		void printVersions();


		void loadClients();
		void printClients();

		void loadFormats();
		void printFormats();

		bool saveTrack(const char* unit_id, int id, int version, const char* buffer);
		bool saveTrack(const char* unit_id, const char* buffer);

		bool saveEvent(const char* unit_id, int type_id);

		bool isVersion(int value);

		std::string createCommand(CMDMsg* msg, unsigned int unitId, unsigned short cmdId);
		std::string createCommand(unsigned int unitId, unsigned short commandId, std::string tag, std::list<string> params, unsigned short mode);
		std::string loadCommand(CMDMsg* msg, unsigned int historyId);
		InfoClient getInfoClient(string id);

		void deviceConfig(const char* unit_id, CommandResult*  result);
		void save(std::string query);

		std::string addPending(unsigned int unitId, unsigned short commandId, unsigned int tag, std::string command, std::string user);
		unsigned int getTag(unsigned int unitId, unsigned short commandId);
		

	private:
		bool debug;
		InfoDB info;

		Versions versions[50];
		int nVersions;
		sql::Driver* driver = nullptr;
		sql::Connection* cn = nullptr;
		sql::Statement* stmt = nullptr;
		sql::ResultSet* result = nullptr;

		std::map<int, InfoProto> mProtocols;
		std::map<std::string, InfoClient> mClients;
		std::map<int, std::list<std::string>> mFormats;
		std::list<int> mVersions;
	};
}
