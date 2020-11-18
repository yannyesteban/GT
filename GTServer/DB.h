#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define MULTILINE(...) #__VA_ARGS__
#include "Color.h"
#include "Types.h"
#include "Tool.h"
#include "ConsoleColors.h"
#include <mysql/jdbc.h>
#include <string>
//
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filereadstream.h"
template <class Container>
void split2(const std::string& str, Container& cont, char delim = ' ');
template <class Container>
void split2(const std::string& str, Container& cont, char delim) {
	std::stringstream ss(str);
	std::string token;
	while (std::getline(ss, token, delim)) {
		cont.push_back(token);
	}
}
namespace GT {
	struct InfoProto {
		
		int id_device;
		int tag_length;
		std::string pass_default;
		std::string protocol_pre;
		std::string token_ok;
		std::string token_error;
		std::string token_resp;
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
		~DB();
		void init();
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

		std::string createCommand(RCommand* request, std::list<string> params);

		std::string loadCommand(CMDMsg* msg, unsigned int historyId);
		InfoClient getInfoClient(string id);

		void deviceConfig(const char* unit_id, CommandResult*  result);
		void getPending(const char* unit_id, CommandResult* commandResult, RCommand* response);
		void save(std::string query);

		std::string addPending(unsigned int unitId, unsigned short commandId, unsigned int tag, std::string command, std::string user, unsigned short type, unsigned short level);
		std::string addPending(RCommand * request);
		void infoCommand(const char* unit_id, CommandResult* commandResult, RCommand* info);
		void saveResponse(RCommand* info, const char* command);

		unsigned int getTag(unsigned int unitId, unsigned short commandId, unsigned int type);
		void evalPending(const char* unit_id, CommandResult* result, unsigned int type);
		bool isReadCommand(const char* unit_id, CommandResult* result);
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

		sql::PreparedStatement* stmtInfoClient;
	};
}
