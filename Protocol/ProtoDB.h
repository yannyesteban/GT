#pragma once
//#define _CRT_SECURE_NO_WARNINGS
#define MULTILINE(...) #__VA_ARGS__
#include <ctime>
#include "Color.h"
#include "Types.h"

#include "ConsoleColors.h"
#include <mysql/jdbc.h>
#include <string>
//
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filereadstream.h"

#include "GTServer/Tool.h"


#include "Cmd.h"


template <class Container>
void split22(const std::string& str, Container& cont, char delim = ' ');
template <class Container>
void split22(const std::string& str, Container& cont, char delim) {
	std::stringstream ss(str);
	std::string token;
	while (std::getline(ss, token, delim)) {
		cont.push_back(token);
	}
}
namespace GT {

	struct InfoEvent {
		int unitId;
		int eventId;
		int mode;
		std::string title;
		std::string info;
		std::string user;
		unsigned short status;
	};


	struct DBEvent1 {
		int unitId;
		char dateTime[20] = "";
		int eventId;
		int mode;
		char title[1024] = "";
		char info[1024] = "";
		char user[45] = "";
		unsigned short status;
	};

	struct EncodeCommand {
		int unitId;
		std::string command;
		int index;
		std::string deviceId;
		std::vector <std::string> params;
		std::string listParams;
		std::string listOParams;
		
		bool indexed = false;

		std::string role;
		std::string label;
		std::string descrption;
		bool usePass = true;
		int level = 0;

		std::string commandExp;
		std::string packlen;
		std::string checksum;
		std::string pass;
		std::string packno;
		int packnoBegin = 1;
		int packnoEnd = 100;
		int deltaIndex = 0;
		
	};
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
		std::string command;
		int index;
		int mode;
		int status;
		
		std::string name;
		std::string params;
		std::string query;
		std::string values;
		std::string user;
		

		

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

		InfoClient getInfoClient(std::string id);

		ClientProto decodeProto(std::string);
		std::map<int, ClientProto> loadProtocols();
		bool saveTrack(int unitId, std::map<std::string, std::string> data);
		bool updateCommand(int unitId, std::string command, int index, std::string value);
		
		std::map<std::string, std::string> decodeExp(std::string s, ClientProto& proto);

		std::map<std::string, std::string> decodeCommand(std::string s, ClientProto& proto);
		
		std::map<std::string, std::string> decodeTracking(std::string s, ClientProto& proto);
		
		std::vector<std::string> decodeParams(std::string s, ClientProto& proto);
		std::string encodeToArray(std::vector<std::string>);


		void evalData(std::map<std::string, std::string>& data, ClientProto & proto);

		std::string encodeJson(std::map<std::string, std::string> data);

		bool isTrack(std::string data, ClientProto& proto);
		bool isCommand(std::string data, ClientProto& proto);


		std::vector<std::string> decodeArray(std::string json);

		EncodeCommand infoCommand(int unitId, std::string command, int index);
		std::map<int, ClientProto> mProto;

		std::vector<int> loadSynchValue();
		std::vector<std::string> loadIdentExp();

		void insertEvent(InfoEvent& infoEvent);

	private:
		bool debug;
		InfoDB info;
		Cmd cmd;
		
		
		
		sql::Driver* driver = nullptr;
		sql::Connection* cn = nullptr;
		sql::Statement* stmt = nullptr;
		sql::ResultSet* result = nullptr;


		sql::PreparedStatement* stmtSaveCommand = nullptr;
		sql::PreparedStatement* stmtUpdateCommand = nullptr;

		
		sql::PreparedStatement* stmtEncodeCommand = nullptr;

		
		sql::PreparedStatement* stmtLoadProtocols = nullptr;
		sql::PreparedStatement* stmtInfoClient = nullptr;
		sql::PreparedStatement* stmtInfoCommand = nullptr;
		sql::PreparedStatement* stmtInsertTracking = nullptr;

		sql::PreparedStatement* stmtSynchValue = nullptr;
		sql::PreparedStatement* stmtIdentExp = nullptr;
		
		sql::PreparedStatement* stmtEvent = nullptr;

		bool initialized = false;
	};
}
