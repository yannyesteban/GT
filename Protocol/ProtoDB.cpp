#include "ProtoDB.h"

using namespace rapidjson;
using namespace std;
std::vector<string> splitParams(const string& s, char delim) {
	std::vector<string> result;
	stringstream ss(s);
	string item;

	while (getline(ss, item, delim)) {
		result.push_back(item);
	}

	return result;
}
namespace GT {
	
	ProtoDB::ProtoDB(InfoDB pInfo):
			info(pInfo),
			debug(pInfo.debug),
			
			driver(nullptr),
			cn(nullptr),
			stmt(nullptr),
			result(nullptr)
		{
		
		driver = get_driver_instance();
	}

	ProtoDB::~ProtoDB() {
		//delete stmtInfoClient;
	}
	
	bool ProtoDB::connect() {
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
				/*
				sql::ConnectOptionsMap cnProp;
				cnProp["hostName"] = info.host;
				
				cnProp["password"] = info.pass;

				cnProp["userName"] = info.user;
				cnProp["scheme"] = info.name;
				cnProp["port"] = info.port;
				cnProp["OPT_RECONNECT"] = info.port;
				*/

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
				
				cout << "Mysql has connected correctaly " << cn->isValid() << endl;
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
			cout << "Mysql has connected correctaly, db: " << info.name << endl;
			init();
			return true;

		} catch (sql::SQLException& e) {
			SQLException(e, __LINE__);
		}

		return false;
	}
	
	void ProtoDB::init() {

		if (initialized) {
			return;
		}
		
		initialized = true;

		string query = "";
		
		
	}

	bool ProtoDB::isAlive() {


		bool value = false;
		if (cn) {
			value = cn->isValid();

		}
		if (value) {
			std::cout << "\n** Mysql is alive\n";
		}
		else {
			std::cout << "\n** Mysql is Dead\n";
		}
		return value;

		
	}

	bool ProtoDB::isValid() {
		isAlive();
		if (cn == NULL) {
			return false;
		}
		cn->reconnect();

		return cn->isValid();
	}

	void ProtoDB::reset() {

		stmtSaveCommand = nullptr;
		stmtEncodeCommand = nullptr;
		stmtLoadProtocols = nullptr;
		stmtInfoClient = nullptr;
		stmtInsertTracking = nullptr;

		
		initialized = false;
	}

	void ProtoDB::SQLException(sql::SQLException& e, long line) {

		cout << "-> ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << line << endl;
		cout << "->  ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		//cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}

	void ProtoDB::SQLException(sql::SQLException& e) {
		
		cout << "->  ERROR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "->  ERROR - : " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode() << endl;
		//cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}

	
	int ProtoDB::saveCmd(InfoCmd info)
	{
		if (!connect()) {
			return 0;
		}
		try {
			if (stmtSaveCommand == nullptr) {
				stmtSaveCommand = cn->prepareStatement(
					R"(INSERT INTO unit_cmd
					(`unit_id`, `command_id`, `index`, `mode`, `name`, `params`, `query`, `status`, `user`)
						VALUES
						(?, ?, ?, ?, ?, ?, ?, ?, ?)
						ON DUPLICATE KEY UPDATE
						`mode` = ?,
						`name` = COALESCE(?, `name`),
						`params` = COALESCE(?, `params`),
						`query` = COALESCE(?, `query`),
						`user` = ?

				)");
			}
			
			sql::ResultSet* result = nullptr;

			stmtSaveCommand->setInt(1, info.unitId);
			stmtSaveCommand->setInt(2, info.commandId);
			stmtSaveCommand->setInt(3, info.index);
			stmtSaveCommand->setInt(4, info.mode);
			stmtSaveCommand->setString(5, info.name.c_str());
			if (info.mode == 1) {
				stmtSaveCommand->setString(6, info.params.c_str());
				stmtSaveCommand->setNull(7, sql::DataType::VARCHAR);

				stmtSaveCommand->setString(12, info.params.c_str());
				stmtSaveCommand->setNull(13, sql::DataType::VARCHAR);
			}
			else {
				stmtSaveCommand->setNull(6, sql::DataType::VARCHAR);
				stmtSaveCommand->setString(7, info.params.c_str());
				
				stmtSaveCommand->setNull(12, sql::DataType::VARCHAR);
				stmtSaveCommand->setString(13, info.params.c_str());
				
			}
			
			stmtSaveCommand->setInt(8, info.status);
			stmtSaveCommand->setString(9, info.user.c_str());

			stmtSaveCommand->setInt(10, info.mode);
			if (info.name != "") {
				stmtSaveCommand->setString(11, info.name.c_str());
			}
			else {
				stmtSaveCommand->setNull(11, sql::DataType::VARCHAR);
			}
			
			stmtSaveCommand->setString(14, info.user.c_str());

			stmtSaveCommand->execute();

			
		}
		catch (sql::SQLException& e) {
			SQLException(e, __LINE__);
		}

		return 0;
	}

	int ProtoDB::updateCmd(InfoCmd info)
	{
		return 0;
	}

	std::string ProtoDB::commandEncode(int unitId, int commandId, int index, int mode, std::string params)
	{
		
		if (!connect()) {
			return "";
		}
		try {
			if (stmtEncodeCommand == nullptr) {
				stmtEncodeCommand = cn->prepareStatement(
					R"(SELECT d.name as device_name, r.id as roleId, w_index,

					CASE uc.mode WHEN 1 THEN uc.params WHEN 2 THEN uc.query END as params,
					uc.mode,
					CONCAT(protocol_pre, command) as str_command,
					CASE WHEN use_pass > 0 THEN d.password ELSE '' end as password,

					command as command_name,
					COALESCE(r.role, c.command) as role, use_pass, use_tag, v.token_ok as pro_mode,
					protocol

					FROM unit_cmd as uc
					INNER JOIN device_command as c ON c.id = uc.command_id
					INNER JOIN device_version as v ON v.id = c.version_id
					INNER JOIN device as d ON d.version_id = v.id
					INNER JOIN unit as u ON u.id = uc.unit_id AND u.device_id = d.id
					LEFT JOIN command_role as r ON r.id = c.role_id

					WHERE uc.unit_id = ? AND uc.command_id = ? AND uc.index = ?

				)");
			}

			sql::ResultSet* result = nullptr;

			stmtEncodeCommand->setInt(1, unitId);
			stmtEncodeCommand->setInt(2, commandId);
			stmtEncodeCommand->setInt(3, index);
			//stmtUnitCommand->setInt(4, mode);

			stmtEncodeCommand->execute();

			if (stmtEncodeCommand->execute()) {
				result = stmtEncodeCommand->getResultSet();
				std::string deviceName = "";
				std::string params = "";
				//std::string command = "";
				std::string commandName = "";
				std::string password = "";
				std::string value = "";
				std::string protoMode = "";
				std::string listParams = "";
				unsigned short useTag = 0;

				std::string encode = "";
				int wIndex = 0;
				int modeCommand = 0;
				int deltaIndex = 0;
				if (result->next()) {
					
					listParams = "";
					encode = "";

					wIndex = result->getInt("w_index");

					deviceName = result->getString("device_name").c_str();
					params = result->getString("params").c_str();
					//command = result->getString("str_command").c_str();
					password = result->getString("password").c_str();
					std::string role = result->getString("role").c_str();

					role = result->getInt("roleId");

					protoMode = result->getString("pro_mode").c_str();
					commandName = result->getString("command_name").c_str();
					modeCommand = result->getInt("mode");
					useTag = (unsigned short)result->getInt("use_tag");
					
					std:string proto = result->getString("protocol").c_str();
					//auto j = Json(proto.c_str());

					

					std::map<std::string, std::string> data;

					if (params != "") {
						Document document;
						document.Parse(params.c_str());
						if (!document.IsArray()) {

							printf(" error en comando !\n");
							delete result;
							return "error";
						}

						const Value& a = document.GetArray();

						for (SizeType i = 0; i < a.Size(); i++) // Uses SizeType instead of size_t

						{
							if (a[i].IsInt()) {
								value = std::to_string(a[i].GetInt());
							}
							else if((a[i].IsString())) {
								value = a[i].GetString();
							}
							if (listParams != "") {
								listParams += "," + value;
							}
							else {
								listParams = value;
							}
						}
						if (listParams != "") {
							data["params"] = "," + listParams;
						}
						else {
							data["params"] = "";
						}
						
						data["oparams"] = listParams;
							
					}

					Document doc;
					doc.Parse(proto.c_str());

					
					data["id"] = result->getString("device_name").c_str();;
					data["pass"] = result->getString("password").c_str();
					data["cmd"] = result->getString("command_name").c_str();
					
					data["fpacklen"] = "";
					data["fchecksum"] = "";
					data["index"] = std::to_string(index);

					//data["packno"] = std::string(1, packNo % 256);
					

					if (wIndex) {
						encode = doc["encode"]["icmd"].GetString();
					}
					else {
						encode = doc["encode"]["cmd"].GetString();
					}
					
					if (doc["encode"].HasMember("fchecksum")) {
						data["fchecksum"] = doc["encode"]["fchecksum"].GetString();
					}

					if (doc["encode"].HasMember("fpacklen")) {
						data["fpacklen"] = doc["encode"]["fpacklen"].GetString();
					}

					if (doc["encode"].HasMember("delta-index")) {
						deltaIndex = doc["encode"]["delta-index"].GetInt();
						data["index"] = std::string(1, index + deltaIndex % 256);
					}

					if (doc["encode"].HasMember("packno")) {
						
						int min = doc["encode"]["packno"][0].GetInt();
						int max = doc["encode"]["packno"][1].GetInt();
						
						static char packNo = min;

						
						if (packNo < min) {
							packNo = min;
						}
						if (packNo > max) {
							packNo = max;
						}

						

						packNo++;

						data["packno"] = std::string(1, packNo % 256);
						
					}

					//std::string str = cmd.encode(encode, data);
					
					delete result;
					return cmd.encode(encode, data);
					
				}
			}

			
		}
		catch (sql::SQLException& e) {
			SQLException(e, __LINE__);
		}
		
		return std::string();
	}

	InfoClient ProtoDB::getInfoClient(string id) {
		if (!connect()) {
			return InfoClient();
		}

		int unit_id = 0, version_id = 0, device_id = 0;
		InfoClient info = { 0,0,0,0,"" };
		try {


			if (stmtInfoClient == nullptr) {

				stmtInfoClient = cn->prepareStatement(
					R"(SELECT u.id as unit_id, d.id as device_id, d.name as device_name, version_id, n.name, format_id
				FROM unit as u
				INNER JOIN device as d on d.id = u.device_id
				INNER JOIN unit_name as n ON n.id = u.name_id
				INNER JOIN device_version as v on v.id = d.version_id
				WHERE d.name = ?)");
			}
			sql::ResultSet* result;

			stmtInfoClient->setString(1, id.c_str());

			if (stmtInfoClient->execute()) {
				result = stmtInfoClient->getResultSet();

				while (result->next()) {
					info.unit_id = result->getInt("unit_id");
					info.device_id = result->getInt("device_id");
					info.version_id = result->getInt("version_id");
					info.format_id = result->getInt("format_id");
					strcpy_s(info.name, sizeof(info.name), result->getString("name").c_str());
				}

				delete result;
			}


		}
		catch (sql::SQLException& e) {
			SQLException(e, __LINE__);
		}



		return info;
	}

	
	void ProtoDB::loadProtocols() {

		if (!connect()) {
			return;
		}

		try {

			if (stmtLoadProtocols == nullptr) {
				stmtLoadProtocols = cn->prepareStatement(R"(
					SELECT id, tag_length, pass_default as pass, protocol_pre,sync_header, format_id, 
					token_ok, token_error,token_resp, protocol
					FROM device_version as d 
				)");
			}

			sql::ResultSet* result;

			if (stmtLoadProtocols->execute()) {
				result = stmtLoadProtocols->getResultSet();
				int id;
				std::string pass;
				std::string protocol;
				mProto.clear();
				while (result->next()) {
					id = result->getInt("id");
					protocol = result->getString("protocol");



					mProto[id] = decodeProto(protocol.c_str());

				}

				delete result;

				

			}
			//std::cout << " " << mProto[1].trackingFields["input_status"].type << "\n";

		}
		catch (sql::SQLException& e) {
			SQLException(e, __LINE__);
		}

	}
	
	ClientProto ProtoDB::decodeProto(std::string jsonDoc)
	{
		ClientProto proto;
		Document doc;
		doc.Parse(jsonDoc.c_str());

		if (!doc.IsObject()) {
			return proto;
		}

		if (doc.HasMember("name")) {
			proto.protoName = doc["name"].GetString();
		}

		if (doc.HasMember("pass")) {
			proto.pass = doc["pass"].GetString();
		}

		if (doc.HasMember("header")) {
			proto.header = doc["header"].GetInt();
		}
		if (doc.HasMember("decode")) {
			
			Value& decode = doc["decode"];
				
			if (decode.IsObject()) {
				if (decode.HasMember("tracking")) {
					proto.tracking = decode["tracking"].GetString();
				}

				if (decode.HasMember("tracking_elem")) {
					proto.trackingNames.clear();

					const Value& arr = decode["tracking_elem"].GetArray();

					for (SizeType i = 0; i < arr.Size(); i++) // Uses SizeType instead of size_t
					{
						proto.trackingNames.push_back(arr[i].GetString());
						
					}
				}

				if (decode.HasMember("tracking_field")) {
					const Value& fields = decode["tracking_field"].GetArray();
					proto.trackingFields.clear();
					if (fields.IsArray()) {
						for (SizeType i = 0; i < fields.Size(); i++) // Uses SizeType instead of size_t
						{
							std::string name = "";
							std::string type = "";
							std::string to = "";

							if (fields[i].HasMember("name")) {
								name = fields[i]["name"].GetString();
							}
							if (fields[i].HasMember("type")) {
								type = fields[i]["type"].GetString();
							}
							if (fields[i].HasMember("to")) {
								type = fields[i]["to"].GetString();
							}

							proto.trackingFields[name] = {
								name, type, to
							};
						}
					}
				}

				if (decode.HasMember("cmd")) {
					proto.decCmd = decode["cmd"].GetString();
				}
				if (decode.HasMember("cmd_element")) {
					proto.decCmdNames.clear();

					const Value& arr = decode["cmd_element"].GetArray();

					for (SizeType i = 0; i < arr.Size(); i++) // Uses SizeType instead of size_t
					{
						proto.decCmdNames.push_back(arr[i].GetString());

					}
				}

				if (decode.HasMember("icmd")) {
					proto.decICmd = decode["icmd"].GetString();
				}
				if (decode.HasMember("icmd_element")) {
					proto.decICmdNames.clear();

					const Value& arr = decode["icmd_element"].GetArray();

					for (SizeType i = 0; i < arr.Size(); i++) // Uses SizeType instead of size_t
					{
						proto.decICmdNames.push_back(arr[i].GetString());

					}
				}

				if (decode.HasMember("delta_index")) {
					proto.decDeltaIndex = decode["delta_index"].GetInt();
				}

				if (decode.HasMember("param_sep")) {
					proto.paramSep = decode["param_sep"].GetString()[0];
				}
			}
		}

		if (doc.HasMember("encode")) {

			Value& encode = doc["encode"];

			if (encode.IsObject()) {
				if (encode.HasMember("cmd")) {
					proto.cmd = encode["cmd"].GetString();
				}
				if (encode.HasMember("icmd")) {
					proto.iCmd = encode["icmd"].GetString();
				}
				if (encode.HasMember("fpacklen")) {
					proto.fpacklen = encode["fpacklen"].GetString();
				}
				if (encode.HasMember("fchecksum")) {
					proto.fchecksum = encode["fchecksum"].GetString();
				}
				if (encode.HasMember("packno_begin")) {
					proto.packnoBegin = encode["packno_begin"].GetInt();
				}
				if (encode.HasMember("packno_end")) {
					proto.packnoEnd = encode["packno_end"].GetInt();
				}
				if (encode.HasMember("delta_index")) {
					proto.deltaIndex = encode["delta_index"].GetInt();
				}
			}
		}

		return proto;
	}


	bool ProtoDB::saveTrack(int unitId, std::map<std::string, std::string> data) {

		if (!connect()) {
			return false;
		}
		
		try {
			std::vector<string> fields = {

				"unit_id", "device_id", "date_time", "longitude", "latitude",
				"speed", "heading", "altitude", "satellite", "event_id",
				"mileage", "input_status", "voltage_level_i1", "voltage_level_i2", "output_status",
				"pulse_i3", "pulse_i4", "rtc", "tag_id", "tag_battery",
				"tag_button_id", "battery_voltage", "voltage_output", "info"
			};

			if (stmtInsertTracking == nullptr) {

				stmtInsertTracking = cn->prepareStatement(
					R"(INSERT IGNORE INTO tracking 

				(unit_id, device_id, date_time, longitude, latitude,
				speed, heading, altitude, satellite, event_id, mileage,
				input_status, voltage_level_i1, voltage_level_i2, output_status,
				pulse_i3, pulse_i4, rtc, tag_id, tag_battery, tag_button_id,
				battery_voltage, voltage_output, info)
				VALUES
				(?,?,?,?,?,
				 ?,?,?,?,?,
				 ?,?,?,?,?,
				 ?,?,?,?,?,
				 ?,?,?,?)
				
				)");
			}
			stmtInsertTracking->setInt(1, unitId);

			for (int i = 1; i < fields.size(); i++) {
				if (data[fields[i]] == "") {
					if (fields[i] == "date_time" || fields[i] == "longitude" || fields[i] == "longitude" ||
						fields[i] == "latitude" || fields[i] == "speed" || fields[i] == "heading" ||
						fields[i] == "altitude" || fields[i] == "satellite" || fields[i] == "event_id" ||
						fields[i] == "input_status" || fields[i] == "voltage_level_i1" || fields[i] == "voltage_level_i2" ||
						fields[i] == "pulse_i3" || fields[i] == "pulse_i4" || fields[i] == "battery_voltage" ||
						fields[i] == "voltage_output")
					{
						stmtInsertTracking->setNull(i + 1, sql::DataType::INTEGER);
					}
					else {
						stmtInsertTracking->setNull(i + 1, sql::DataType::VARCHAR);
					}
				}
				else {
					stmtInsertTracking->setString(i + 1, data[fields[i]].c_str());
				}
				std::cout << "\n i: " << i << " - " << fields[i] << " ";
			}
			
			stmtInsertTracking->execute();
		}

		catch (sql::SQLException& e) {
			SQLException(e, __LINE__);
		}

		return true;
	}

	std::map<std::string, std::string> ProtoDB::decodeExp(std::string s, ClientProto& proto) {

		regex regexp(proto.tracking);
		auto names = proto.trackingNames;
		smatch m;


		std::map<std::string, std::string> map;

		regex_search(s, m, regexp);

		for (int i = 1; i < m.size(); i++) {
			if (i < names.size()) {
				map[names[i - 1]] = m[i].str();
				cout << "\nm [" << names[i - 1] << "]: " << m[i].str();
			}
		}
		printf("\n\n");

		for (std::map<std::string, std::string>::iterator it = map.begin(); it != map.end(); ++it) {
			printf("%20s", it->first.c_str());
			printf("%50s\n", it->second.c_str());

		}

		return map;
	}
	
	std::map<std::string, std::string> ProtoDB::decodeCommand(std::string s, ClientProto& proto) {


		//s = "&&532,5024000001,252,yan,est,nue,1247E4";
		regex regexp("&&(\\w)(\\d+),(\\w+),(\\w+)(?:,(.+))?(\\w{2})");
		auto names = proto.trackingNames;
		names = std::vector<std::string>{"index","packlen","device_id","command","params","checksum"};
		smatch m;


		std::map<std::string, std::string> map;

		regex_search(s, m, regexp);

		for (int i = 1; i < m.size(); i++) {
			printf("----\n\n");
			if (i <= names.size()) {
				map[names[i - 1]] = m[i].str();
				cout << "\nm [" << names[i - 1] << "]: " << m[i].str();
			}
		}
		printf("sixe: %d\n%s...\n\n", m.size(), s.c_str());

		for (std::map<std::string, std::string>::iterator it = map.begin(); it != map.end(); ++it) {
			printf("%20s", it->first.c_str());
			printf("%50s\n", it->second.c_str());

		}

		return map;
	}

	std::vector<std::string> ProtoDB::decodeParams(std::string s, ClientProto& proto) {
		
		return splitParams(s, proto.paramSep);
		
	}

	std::map<std::string, std::string> ProtoDB::decodeTracking(std::string s, ClientProto& proto) {

		regex regexp(proto.tracking);
		auto names = proto.trackingNames;
		smatch m;


		std::map<std::string, std::string> map;

		regex_search(s, m, regexp);

		for (int i = 1; i < m.size(); i++) {
			if (i < names.size()) {
				map[names[i - 1]] = m[i].str();
				cout << "\nm [" << names[i - 1] << "]: " << m[i].str();
			}
		}
		printf("\n\n");

		for (std::map<std::string, std::string>::iterator it = map.begin(); it != map.end(); ++it) {
			printf("%20s", it->first.c_str());
			printf("%50s\n", it->second.c_str());

		}

		return map;
	}



	
	
	void ProtoDB::evalData(std::map<std::string, std::string> & data, ClientProto & proto)
	{
		
		printf("\n\n");

		std::map<std::string, std::string> info;

		for (std::map<std::string, std::string>::iterator it = data.begin(); it != data.end(); ++it) {
			auto field = proto.trackingFields[it->first.c_str()];
			if (field.name != "") {
				if (field.type == "hex") {
					unsigned int value;
					std::stringstream ss;
					ss << std::hex << it->second.c_str();
					ss >> value;
					it->second = std::to_string(value);
				}
				if (field.type == "info") {
					info[field.name] = it->second.c_str();
				}
			}
			//std::cout << field.name << "\n";
			
			printf("%20s", it->first.c_str());
			printf("%50s\n", it->second.c_str());

			
		}
		
		data["info"] = encodeJson(info);
	}

	std::string ProtoDB::encodeJson(std::map<std::string, std::string> data) {
		rapidjson::Document json;
		json.SetObject();


		std::string message;
		rapidjson::Value msg;

		for (std::map<std::string, std::string>::iterator it = data.begin(); it != data.end(); ++it) {
			printf("%20s", it->first.c_str());
			printf("%50s\n", it->second.c_str());
			message = it->second;
			rapidjson::Value key(it->first.c_str(), it->first.size(), json.GetAllocator());
			msg.SetString(message.c_str(), message.size(), json.GetAllocator());
			json.AddMember(key, msg, json.GetAllocator());

		}



		rapidjson::StringBuffer sbuffer;
		rapidjson::Writer<rapidjson::StringBuffer> wr(sbuffer);

		json.Parse(sbuffer.GetString());

		json.Accept(wr);

		std::cout << " JSON : " << sbuffer.GetString() << "\n\n";
		return sbuffer.GetString();
	}
	bool ProtoDB::isTrack(std::string s, ClientProto& proto)
	{
		std::regex e(proto.tracking);

		if (std::regex_match(s, e)) {
			std::cout << "string object matched\n";
			return true;
		}
			
		
		return false;
	}
	std::string ProtoDB::encodeToArray(std::vector<string> params)
	{

		rapidjson::Document json;
		json.SetArray();

		Document::AllocatorType& allocator = json.GetAllocator();

		std::vector<std::string> result;
		for (std::vector<string>::iterator it = params.begin(); it != params.end(); it++) {

			rapidjson::Value key(it->c_str(), it->size(), allocator);
			json.PushBack(key, allocator);
			
		}

		rapidjson::StringBuffer sbuffer;
		rapidjson::Writer<rapidjson::StringBuffer> wr(sbuffer);

		json.Parse(sbuffer.GetString());

		json.Accept(wr);

		//std::cout << " JSON-STRING-ARRAY: " << sbuffer.GetString() << "\n";
		return sbuffer.GetString();
	}
}