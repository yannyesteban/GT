#pragma once
#include <mysql/jdbc.h>
#include <vector>
#include <string>
#include "Color.h"
#include "Types.h"


#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filereadstream.h"
namespace GT {
	struct TrackingField2 {
		int pos = 0;
		int type = 0;
	};
	struct UnitInfo {
		int unitId;
		std::string unitName;
	};
	class DB2 {
	public:
		DB2(InfoDB pInfo);
		~DB2();
		void init();
		void reset();
		bool connect();
		void test();
		void SQLException(sql::SQLException& e);
		void SQLException(sql::SQLException& e, long line);
		void getInfoUnit(int unitId, UnitInfo* unitInfo);
		void loadUnits(std::vector<int> * Devices);
		void loadFormat(int unitId, std::vector<std::string>* format);
		std::string loadTracking(int unitId, int * id, std::vector<std::string> format);
	private:
		InfoDB info;

		Versions versions[50];
		int nVersions;

		std::map<std::string, TrackingField2> mTrackingField;

		sql::Driver* driver = nullptr;
		sql::Connection* cn = nullptr;
		sql::Statement* stmt = nullptr;
		sql::ResultSet* result = nullptr;

		sql::PreparedStatement* stmtInfoUnit = nullptr;
		sql::PreparedStatement* stmtLoadUnits = nullptr;
		sql::PreparedStatement* stmtLoadFormat = nullptr;
		sql::PreparedStatement* stmtLoadTracking = nullptr;
		
		bool initialized = false;
	};
}