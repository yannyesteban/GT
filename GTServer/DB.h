#pragma once
#include "Types.h"
#include "Tool.h"
#include <mysql/jdbc.h>
#include<string>
//

namespace GT {
	struct InfoProto {
		
		int id_device;
		int tag_length;
		std::string pass_default;
		std::string protocol_pre;
		std::string sync_header;

	};

	struct InfoClient {
		int id;
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

		bool isVersion(int value);

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
