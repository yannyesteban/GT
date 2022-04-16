#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define MULTILINE(...) #__VA_ARGS__
#include <ctime>
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


#include "Cmd.h"


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

	struct FieldConfig {
		std::string name;
		std::string type;
		std::string to;
	};

	struct ClientProto {
		std::string tracking ;
		std::vector<std::string> trackingNames;
		std::map<std::string, FieldConfig> trackingFields;

		std::string cmd;
		std::string iCmd;
		std::string fpacklen;
		std::string fchecksum;
		std::string pass;
		std::string protoName;

		std::string decCmd;
		std::string decICmd;
		std::vector<std::string> decCmdNames;
		std::vector<std::string> decICmdNames;

		char paramSep;
		int packnoBegin = 1;
		int packnoEnd = 100;
		int deltaIndex = 0;
		int decDeltaIndex = 0;
		int header = 0;
		int unitId;
		int deviceId;
		int versionId;
		
		std::string name;
	};

	struct InfoCmd {

		int unitId;
		int commandId;
		int index;
		int mode;
		int status;
		std::string name;
		std::string params;
		std::string query;
		std::string values;
		std::string user;
		

		

	};

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

	struct InfoPending {
		unsigned int id;
		unsigned int unitId;
		unsigned int commandId;
		std::string command;
		std::string name;
		unsigned short level;
		unsigned short type;
		unsigned short mode;
		std::string user;

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
	struct TrackingField {
		int pos = 0;
		int type = 0;
	};
	struct InfoProto3 {

		int id_device;
		int tag_length;
		char* pass_default;
		char* protocol_pre;
		char* sync_header;

	};

	struct DBEvent {
		int unitId;
		char dateTime[20] = "";
		int eventId;
		int mode;
		char title[1024] = "";
		char info[1024] = "";
		char user[45] = "";
		unsigned short status;
	};


	class ProtoDB {
	public:
		ProtoDB(InfoDB pInfo);
		~ProtoDB();
		void init();
		bool connect();
		bool isValid();
		bool isAlive();
		void reset();
		void SQLException(sql::SQLException& e);
		void SQLException(sql::SQLException& e, long line);

		


		int saveCmd(InfoCmd info);
		int updateCmd(InfoCmd info);

		std::string commandEncode(int unitId, int commandId, int index, int mode, std::string params);

		InfoClient getInfoClient(string id);

		ClientProto decodeProto(std::string);
		void loadProtocols();
		bool saveTrack(int unitId, std::map<std::string, std::string> data);
		
		std::map<std::string, std::string> decodeExp(std::string s, ClientProto& proto);

		std::map<std::string, std::string> decodeCommand(std::string s, ClientProto& proto);
		
		std::map<std::string, std::string> decodeTracking(std::string s, ClientProto& proto);
		
		std::vector<std::string> decodeParams(std::string s, ClientProto& proto);
		std::string encodeToArray(std::vector<string>);

		void evalData(std::map<std::string, std::string>& data, ClientProto & proto);

		std::string encodeJson(std::map<std::string, std::string> data);

		bool isTrack(std::string data, ClientProto& proto);
		bool isCommand(std::string data, ClientProto& proto);

		std::map<int, ClientProto> mProto;

	private:
		bool debug;
		InfoDB info;
		Cmd cmd;
		
		
		sql::Driver* driver = nullptr;
		sql::Connection* cn = nullptr;
		sql::Statement* stmt = nullptr;
		sql::ResultSet* result = nullptr;


		sql::PreparedStatement* stmtSaveCommand = nullptr;
		sql::PreparedStatement* stmtEncodeCommand = nullptr;

		
		sql::PreparedStatement* stmtLoadProtocols = nullptr;
		sql::PreparedStatement* stmtInfoClient = nullptr;
		sql::PreparedStatement* stmtInsertTracking = nullptr;

		

		bool initialized = false;
	};
}
