#include "DB2.h"
namespace GT {
	DB2::DB2(InfoDB pInfo) :
		info(pInfo),
		nVersions(0),
		driver(nullptr),
		cn(nullptr),
		stmt(nullptr),
		result(nullptr) 		{

		driver = get_driver_instance();
	}

	DB2::~DB2() {
		reset();
	}
	void DB2::reset() {
		delete stmtInfoUnit;
		delete stmtLoadUnits;
		delete stmtLoadFormat;
		delete stmtLoadTracking;

		initialized = false;
	}
	void DB2::init() {
		if (initialized) {
			//return;
		}

		initialized = true;

		stmtLoadUnits = cn->prepareStatement(
			R"(SELECT
				a.*
				FROM unit_active as a
				INNER JOIN user_unit as u ON u.unit_id = a.unit_id
				WHERE u.user='panda'
				)"
		);

		stmtInfoUnit = cn->prepareStatement(R"(
			SELECT u.id as unitId, d.name as unitName
			FROM unit as u
			INNER JOIN unit_name as n ON n.id = u.name_id
			INNER JOIN device as d ON d.id = u.device_id
			WHERE u.id = ?
		)");
		
		

		stmtLoadFormat = cn->prepareStatement(R"(
			SELECT f.parameter
			FROM device_format as f
			INNER JOIN device_version as v ON v.id = f.format_id
			INNER JOIN device as d ON d.version_id = v.id
			INNER JOIN unit as u ON u.device_id = d.id
			WHERE u.id = ?
			ORDER BY f.order)");

		stmtLoadTracking = cn->prepareStatement(R"(
			SELECT t.*, now() as date_time, t.date_time as time2 
			FROM tracking2 as t
			WHERE 
				unit_id = ?
			AND id > ?
			limit 1)");
		
		mTrackingField.insert({ "unit_id", {1 ,1} });
		mTrackingField.insert({ "device_cod", {2 ,1} });
		mTrackingField.insert({ "device_id", {3 ,2} });
		mTrackingField.insert({ "date_time", {4 ,2} });
		mTrackingField.insert({ "longitude", {5 ,3} });
		mTrackingField.insert({ "latitude", {6 ,3} });

		mTrackingField.insert({ "speed", {7 ,1} });
		mTrackingField.insert({ "heading", {8 ,1} });
		mTrackingField.insert({ "altitude", {9 ,1} });
		mTrackingField.insert({ "satellite", {10 ,1} });
		mTrackingField.insert({ "event_id", {11 ,1} });
		mTrackingField.insert({ "mileage", {12 ,1} });

		mTrackingField.insert({ "input_status", {13 ,1} });
		mTrackingField.insert({ "voltage_level_i1", {14 ,3} });
		mTrackingField.insert({ "voltage_level_i2", {15 ,3} });
		mTrackingField.insert({ "output_status", {16 ,1} });

		mTrackingField.insert({ "pulse_i3", {17 ,3} });
		mTrackingField.insert({ "pulse_i4", {18 ,3} });
		mTrackingField.insert({ "rtc", {19 ,1} });
		mTrackingField.insert({ "tag_id", {20 ,1} });
		mTrackingField.insert({ "tag_battery", {21 ,1} });
		mTrackingField.insert({ "tag_button_id", {22 ,1} });

		mTrackingField.insert({ "battery_voltage", {23 ,1} });
		mTrackingField.insert({ "voltage_output", {24 ,1} });
		
	}
	bool DB2::connect() {
		try {

			bool reconnect = false;
			bool connected = false;

			if (cn == NULL) {
				connected = false;
			} else {
				if (cn->isValid()) {
					return true;
				}
				//OutputDebugString(_T("\ncn->isValid() || cn->reconnect()"));
				reconnect = (cn->isValid() || cn->reconnect());
			}

			if (reconnect) {
				reset();
				init();
				return true;
			}


			if (!connected) {

				//reconnect = (cn->isValid() || cn->reconnect());

				char str_host[100] = "tcp://";
				strcat_s(str_host, info.host);
				strcat_s(str_host, ":");
				strcat_s(str_host, info.port);

				cn = driver->connect(str_host, info.user, info.pass);
				connected = cn->isValid();
				bool myTrue = true;
				cn->setClientOption("OPT_RECONNECT", &myTrue);
				cn->setSchema(info.name);

				//std::cout << "Mysql has connected correctaly, db: " << info.name << endl;

				reset();
				init();
				//OutputDebugString(_T("Mysql Conectado"));

			}

			if (connected) {

				std::cout << "Mysql has connected correctaly " << cn->isValid() << std::endl;
				/* Connect to the MySQL test database */

				//OutputDebugString(_T("Mysql Todo bien"));
				return true;
			}



			char str_host[100] = "tcp://";
			strcat_s(str_host, info.host);
			strcat_s(str_host, ":");
			strcat_s(str_host, info.port);

			cn = driver->connect(str_host, info.user, info.pass);

			/* Connect to the MySQL test database */
			cn->setSchema(info.name);
			//cn->isValid()
			std::cout << "Mysql has connected correctaly, db: " << info.name << std::endl;
			init();
			return true;

		} catch (sql::SQLException& e) {
			SQLException(e, __LINE__);
		}

		return false;
	}

	void DB2::test() {
		std::cout << "Test Mysql " << std::endl;
	}

	void DB2::SQLException(sql::SQLException& e, long line) {

		std::cout << "{# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << line << std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode() << "}\n";
		//cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}

	void DB2::getInfoUnit(int unitId, UnitInfo* unitInfo) {
		if (!connect()) {
			return;
		}

		try {

			sql::ResultSet* result;
			stmtInfoUnit->setInt(1, unitId);
			if (stmtInfoUnit->execute()) {
				result = stmtInfoUnit->getResultSet();

				if (result->next()) {
					unitInfo->unitId = result->getInt("unitId");
					unitInfo->unitName = result->getString("unitName").c_str();
				}

				delete result;
			}
		} catch (sql::SQLException& e) {
			SQLException(e, __LINE__);
		}
	}

	

	void DB2::loadUnits(std::vector<int> * Devices) {
		if (!connect()) {
			return;
		}

		try {

			sql::ResultSet* result;
			
			if (stmtLoadUnits->execute()) {
				result = stmtLoadUnits->getResultSet();

				while (result->next()) {
					Devices->push_back(result->getInt("unit_id"));
				}

				delete result;
			}
		} catch (sql::SQLException& e) {
			SQLException(e, __LINE__);
		}
	}

	void DB2::loadFormat(int unitId, std::vector<std::string>* format) {
		
		if (!connect()) {
			return;
		}
		
		try {

			sql::ResultSet* result;
			stmtLoadFormat->setInt(1, unitId);

			if (stmtLoadFormat->execute()) {
				
				result = stmtLoadFormat->getResultSet();
				while (result->next()) {
					format->push_back(result->getString("parameter").c_str());
				}

				delete result;
			}
		} catch (sql::SQLException& e) {
			SQLException(e, __LINE__);
		}
	}

	std::string DB2::loadTracking(int unitId, int * id, std::vector<std::string> format2) {
		

		std::string cmd = "";

		std::vector<std::string> format;
		loadFormat(unitId, &format);
		if (!connect()) {
			return cmd;
		}

		
		try {
			

			sql::ResultSet* result;
			stmtLoadTracking->setInt(1, unitId);
			stmtLoadTracking->setInt(2, *id);

			if (stmtLoadTracking->execute()) {
				
				result = stmtLoadTracking->getResultSet();
				std::string field = "";
				if (result->next()) {
					*id = result->getInt("id");
					std::cout << " ID : " << *id << " time " << result->getString("time2").c_str() << std::endl;
					for (int i = 0; i < format.size(); i++) {
						if (mTrackingField[format[i].c_str()].type == 2) {
							field = result->getString(format[i].c_str()).c_str();
						} else if(mTrackingField[format[i].c_str()].type == 1){
							field = std::to_string(result->getInt(format[i].c_str()));
						} else if (mTrackingField[format[i].c_str()].type ==3) {
							field = std::to_string(result->getDouble(format[i].c_str()));
						}
						
						if (cmd != "") {
							cmd += "," + field;
						} else {
							cmd = field;
						}
					}
				}

				delete result;
			}
		} catch (sql::SQLException& e) {
			SQLException(e, __LINE__);
		}

		return cmd;
	}

	void DB2::SQLException(sql::SQLException& e) {

		std::cout << "# ERROR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << std::endl;
		std::cout << "# ERROR - : " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode() << std::endl;
		//cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
	

}