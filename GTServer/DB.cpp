#include "DB.h"

using namespace rapidjson;
using namespace std;

namespace GT {
	DB::DB(InfoDB pInfo):
		info(pInfo),
		debug(pInfo.debug),
		nVersions(0),
		driver(nullptr),
		cn(nullptr),
		stmt(nullptr),
		result(nullptr) {
	}

	bool DB::connect() {
		try {
			driver = get_driver_instance();

			char str_host[100] = "tcp://";
			strcat_s(str_host, info.host);
			strcat_s(str_host, ":");
			strcat_s(str_host, info.port);

			cn = driver->connect(str_host, info.user, info.pass);
			cout << "Mysql has connected correctaly " << cn->isValid() << endl;
			/* Connect to the MySQL test database */
			cn->setSchema(info.name);
			return 1;

		} catch (sql::SQLException & e) {
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			//cout << ", SQLState: " << e.getSQLState() << " )" << endl;
			return 0;
		}
	}

	void DB::loadProtocols() {
		
		try {
			sql::Statement* stmt;
			sql::ResultSet* result;
			sql::PreparedStatement* p_stmt;

			stmt = cn->createStatement();

			p_stmt = cn->prepareStatement(
				R"(SELECT id, tag_length, pass_default, protocol_pre,sync_header, format_id 
				FROM devices_versions as d )"
			);

			if (p_stmt->execute()) {
				result = p_stmt->getResultSet();

				while (result->next()) {

					mProtocols.insert(std::pair<int, InfoProto >(result->getInt("id"), {
						result->getInt("id"),
						result->getInt("tag_length"),
						result->getString("pass_default").c_str(),
						result->getString("protocol_pre").c_str(),
						result->getString("sync_header").c_str(),
						result->getInt("format_id"),
						}));

				}
				
				delete result;
				delete p_stmt;
				delete stmt;
				if (debug) {
					printProtocols();
				}
				
			}


		} catch (sql::SQLException & e) {
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			//cout << ", SQLState: " << e.getSQLState().c_str() << " )" << endl;
		}

	}

	void DB::printProtocols() {

		printf("\n*** Cache for Devices Version ***\n\n");

		printf("%8s", "Id");
		printf("%8s", "TabLen");
		printf("%10s", "Pass");
		printf("%10s", "Pre");
		printf("%12s\n", "Sync");

		printf("%8s", "/=====");
		printf("%8s", "/=====");
		printf("%10s", "/=======");
		printf("%10s", "/========");
		printf("%12s\n", "/=========");

		for (std::map<int, InfoProto>::iterator it = mProtocols.begin(); it != mProtocols.end(); ++it) {

			printf("%8d", it->second.id_device);
			printf("%8d", it->second.tag_length);
			printf("%10s", it->second.pass_default.c_str());
			printf("%10s", it->second.protocol_pre.c_str());
			printf("%12s\n", it->second.sync_header.c_str());

		}
	}

	void DB::loadVersions() {

		try {
			sql::Statement* stmt;
			sql::ResultSet* result;
			sql::PreparedStatement* p_stmt;

			stmt = cn->createStatement();

			p_stmt = cn->prepareStatement(
				"SELECT sync_dec FROM devices_versions d GROUP BY sync_dec; "
			);

			if (p_stmt->execute()) {
				result = p_stmt->getResultSet();

				while (result->next()) {

					mVersions.push_back(result->getInt("sync_dec"));
					versions->e[versions->n++] = result->getInt("sync_dec");
				}

				delete result;
				delete p_stmt;
				delete stmt;
				if (debug) {
					printVersions();
				}

			}


		} catch (sql::SQLException & e) {
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			//cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		}
	}

	void DB::printVersions() {
		printf("\n*** Cache for Sync Versions ***\n\n");
		printf("%10s", "Id");
		printf("%10s\n", "Sync");
		printf("%10s", "/========");
		printf("%10s\n", "/=========");

		for (int i = 0; i < versions->n; i++) {
			printf("%10d", i);
			printf("%10d\n", versions->e[i]);
		}
	}

	void DB::loadClients() {

		try {
			sql::Statement* stmt;
			sql::ResultSet* result;
			sql::PreparedStatement* p_stmt;

			stmt = cn->createStatement();

			p_stmt = cn->prepareStatement(
				R"(
				SELECT u.id as unit_id, d.id as device_id, device_name, version_id 
				FROM units as u 
				INNER JOIN devices as d on d.id = u.device_id 
				WHERE device_name IS NOT NULL )"
			);

			if (p_stmt->execute()) {
				result = p_stmt->getResultSet();

				while (result->next()) {

					mClients.insert(std::pair<string, InfoClient >(result->getString("device_name").c_str(), {
						result->getInt("unit_id"),
						result->getInt("device_id"),
						result->getInt("version_id")
						}));

				}

				delete result;
				delete p_stmt;
				delete stmt;
				if (debug) {
					printClients();
				}

			}


		} catch (sql::SQLException & e) {
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			//cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		}
	}

	void DB::printClients() {
		printf("\n*** Cache for Sync Versions ***\n\n");
		printf("%12s", "Name");
		printf("%10s", "U Id");
		printf("%10s\n", "Version");
		printf("%12s", "/==========");
		printf("%10s", "/========");
		printf("%10s\n", "/=========");
		for (std::map<std::string, InfoClient>::iterator it = mClients.begin(); it != mClients.end(); ++it) {
			printf("%12s", it->first.c_str());
			printf("%10d", it->second.unit_id);
			printf("%10d\n", it->second.version_id);
		}
	}

	void DB::loadFormats() {
		try {
			sql::Statement* stmt;
			sql::ResultSet* result;
			sql::PreparedStatement* p_stmt;

			stmt = cn->createStatement();

			p_stmt = cn->prepareStatement(
				"SELECT format_id, parameter FROM devices_format as d ORDER BY format_id, `order`;"
			);

			if (p_stmt->execute()) {
				result = p_stmt->getResultSet();
				int version;
				
				while (result->next()) {
					version = result->getInt("format_id");
					mFormats[version].push_back(result->getString("parameter").c_str());
				}

				delete result;
				delete p_stmt;
				delete stmt;
				if (debug) {
					printFormats();
				}

			}


		} catch (sql::SQLException & e) {
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			//cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		}
	
	
	}

	void DB::printFormats() {


		printf("\n*** Cache for Format Tracks ***\n\n");
		printf("%12s", "Id Version");
		printf("%16s\n", "Param");


		int aux = 0;

		for (std::map<int, std::list<std::string>>::iterator it = mFormats.begin(); it != mFormats.end(); ++it) {
			//puts("que");
			//printf("%12d\n", it->first);
			if (aux != it->first) {
				printf("%12s", "/==========");
				printf("%16s\n", "/=============");
				aux = it->first;
			}

			for (std::list<string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
				printf("%12d", it->first);
				printf("%16s\n", it2->c_str());
			}
			/*
			for (int j = 0; j < it->second->n; j++) {
				printf("%12d", it->first);
				printf("%16s\n", it->second->s[j]);
			}
			*/
		}
	}
	
	bool DB::saveTrack(const char* unit_id, const char* buffer) {
		//std::string s(buffer);

		//buffer = "2012000413,20190717161915,-66.845906,10.500806,1,279,983.0,4,2,0.0,1,12.27,0.01,0,0,0,1";

		
		//list<string> field = XT::Tool::split(s, ',');
		int version = mProtocols[ mClients[unit_id].version_id].format_id;

		version = mClients[unit_id].version_id;

		std::cout << "unit_id : " << unit_id << " version(Format) " << version  << endl;
		std::cout << "buffer: " << buffer << std::endl;
		std::string  mm[30];
		int n;
		GT::Tool::getItem(mm, n, buffer);
		std::string query = "INSERT INTO tracking ";
		std::string qfields = "unit_id";
		std::string qvalues(std::to_string(mClients[unit_id].unit_id));

		//char aux[10];
		//sprintf(aux, "'%d'", id);
		//std::string qvalues = std::to_string(n);

		int x = 0;// counter of items
		for (std::list<string>::iterator it = mFormats[version].begin(); it != mFormats[version].end(); it++) {
			if (qfields != "") {
				qfields += ",";
				qvalues += ",";
			}
			qfields = qfields + "`" + it->c_str() + "`";
			qvalues = qvalues + "'" + mm[x++] + "'";
			//std::cout << ":" << qfields << endl << qvalues << endl;
		}
		
		
		query = query + "(" + qfields + ") VALUES (" + qvalues + ");";


		cout << "*****************************" << endl;

		try {
			
			
			printf("" ANSI_COLOR_MAGENTA);
			std::cout << query << endl;
			printf("" ANSI_COLOR_RESET);
			sql::Statement* stmt;
			//sql::ResultSet* res;
			stmt = cn->createStatement();
			stmt->execute(query.c_str());
			//delete res;
			delete stmt;
			

		} catch (sql::SQLException & e) {
			printf("" ANSI_COLOR_BLUE ANSI_COLOR_CYAN_);
			std::cout << query << endl;
			printf("" ANSI_COLOR_RESET);

			
			cout << endl << endl << "# ERR: SQLException in " << __FILE__;
			cout << endl << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << endl << "# ERR: " << e.what();
			cout << endl << " (MySQL error code: " << e.getErrorCode();
			//cout << ", SQLState: " << e.getSQLState() << " )" << endl;

		}
		
		return true;
	}
	
	
	bool DB::saveTrack(const char* unit_id, int id, int version, const char* buffer) {

		//std::string s(buffer);

		buffer = "2012000413,20190717161915,-66.845906,10.500806,1,279,983.0,4,2,0.0,1,12.27,0.01,0,0,0,1";


		//list<string> field = XT::Tool::split(s, ',');

		std::string  mm[30];
		int n;
		GT::Tool::getItem(mm, n, buffer);
		std::string query = "INSERT INTO tracking ";
		std::string qfields = "device_cod";
		std::string qvalues(std::to_string(id));
		
		//char aux[10];
		//sprintf(aux, "'%d'", id);
		//std::string qvalues = std::to_string(n);
		
		int x = 0;// counter of items
		for (std::list<string>::iterator it = mFormats[version].begin(); it != mFormats[version].end(); it++) {
			if (qfields != "") {
				qfields += ",";
				qvalues += ",";
			}
			qfields = qfields + "`" + it->c_str() + "`";
			qvalues = qvalues + "'" + mm[x++] + "'";
		}
		query = query + "(" + qfields + ") VALUES (" + qvalues + ");";
		
		//std::cout << query << endl;
		

		try {
			sql::Statement* stmt;
			sql::ResultSet* res;
			stmt = cn->createStatement();
			//res = stmt->executeQuery(query.c_str());
			//delete res;
			delete stmt;

		} catch (sql::SQLException & e) {

			if (1 == 0) {
				cout << endl << endl << "# ERR: SQLException in " << __FILE__;
				cout << endl << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
				cout << endl << "# ERR: " << e.what();
				cout << endl << " (MySQL error code: " << e.getErrorCode();
				cout << ", SQLState: " << e.getSQLState() << " )" << endl;
			}

		}


		return true;
	}

	bool DB::saveEvent(const char* unit_id, int type_id) {

		printf("unit_id: %s, type: %d\n", unit_id, type_id);

		std::string query = "INSERT INTO units_events ( unit_id, type_id, date_time) "
		 "VALUES ('"+ (std::string)unit_id +"',2,'2020-10-01 11:11:22')";

		printf("query :%s\n", query.c_str());

		try {
			sql::Statement* stmt;
			sql::ResultSet* res;
			stmt = cn->createStatement();
			res = stmt->executeQuery(query.c_str());
			delete res;
			delete stmt;

		} catch (sql::SQLException & e) {

			if (1 == 0) {
				cout << endl << endl << "# ERR: SQLException in " << __FILE__;
				cout << endl << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
				cout << endl << "# ERR: " << e.what();
				cout << endl << " (MySQL error code: " << e.getErrorCode();
				cout << ", SQLState: " << e.getSQLState() << " )" << endl;
			}
			return false;

		}
		return true;
	}

	bool DB::isVersion(int value) {
		for (std::list<int>::iterator it = mVersions.begin(); it != mVersions.end(); it++) {

			
			if (value == *it) {
				printf("is sync..%d...%d....\n", value, *it);
				return true;
			}
		}
		
		return false;
	}

	std::string DB::createCommand(CMDMsg* msg, unsigned int unitId, unsigned short commandId) {

		//CMDMsg* msg = (CMDMsg*)Info.buffer;
		Document document;
		//std::string str = msg->params;
		//cout << msg->params << endl;
		document.Parse((const char*)msg->params);
		//document.Parse(str.c_str());
		if (!document.IsArray()) {
			printf("ERROR JSON...!!!\n");
			return 0;
		}
		SizeType ii = document.Size();
		for (SizeType x = 0; x < ii; x++) {
			cout << x << " ==> " << document[x].GetString() << endl;
		}
		//printf("JSON deviceId %s\n", document[0].GetString());

		std::string command;
		std::string str = "";

		try {
			sql::Statement* stmt;
			sql::ResultSet* result;
			sql::PreparedStatement* p_stmt;

			

			stmt = cn->createStatement();
			string query = "SELECT count(p.id) as n_commands, c.*, CONCAT(protocol_pre, command, '=', d.password) as command1 "
				"FROM devices_commands as c "
				"LEFT JOIN devices_comm_params as p ON p.command_id = c.id "
				"INNER JOIN devices_versions as v ON v.id = c.version_id "
				"INNER JOIN devices as d ON d.version_id = v.id "
				"INNER JOIN units as u ON u.device_id = d.id "

				"WHERE "
				"c.id = '" + to_string(commandId) + "' "
				"and "
				"u.id = '" + to_string(unitId) + "' "
				"order by c.id, `order`;";
			//cout << "query: " << query << endl;
			p_stmt = cn->prepareStatement(query.c_str());
			int n_commands = 0;
			if (p_stmt->execute()) {
				result = p_stmt->getResultSet();

				while (result->next()) {
					n_commands = result->getInt("n_commands");
					command = result->getString("command1").c_str();
					
				}

				delete result;
				delete p_stmt;
				delete stmt;
				if (debug) {
					//printClients();
				}

			}

			//cout << " n Commands " << n_commands << endl;
			str = command;
			if (ii == n_commands) {
				for (SizeType x = 0; x < ii; x++) {
					if (str != "") {
						str = str + "," + document[x].GetString();
					}


					//cout << x << " ==> " << str << endl;
				}
			}

			if (msg->type == 2) {
				str = str + ",?";
			}
			printf("" ANSI_COLOR_CYAN);
			std::cout << "Command: " << str << endl;
			printf("" ANSI_COLOR_RESET);

		} catch (sql::SQLException & e) {
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			//cout << ", SQLState: " << e.getSQLState() << " )" << endl;
			//return "";
		}



		return str;
	}


	std::string DB::createCommand(unsigned int unitId, unsigned short commandId, std::string tag, std::list<string> params, unsigned short type) {

		std::string command;
		std::string str = "";

		try {
			//sql::Statement* stmt;
			sql::ResultSet* result;
			sql::PreparedStatement* p_stmt;



			//stmt = cn->createStatement();
			string query = R"(
				SELECT count(p.id) as n_commands, c.*, CONCAT(protocol_pre, command) as command1, d.password,
					sum(case when p.type='Q' then 1 else 0 end) as qp,
					sum(case when p.type='A' then 1 else 0 end) as ap,
					sum(case when p.type='W' then 1 else 0 end) as wp,
					sum(case when p.type='R' then 1 else 0 end) as rp 
				FROM devices_commands as c 
				LEFT JOIN devices_comm_params as p ON p.command_id = c.id 
				INNER JOIN devices_versions as v ON v.id = c.version_id 
				INNER JOIN devices as d ON d.version_id = v.id 
				INNER JOIN units as u ON u.device_id = d.id 

				WHERE 
				c.id = ? 
				and 
				u.id = ? 
				order by c.id, `order`;)";
			//cout << "query: " << query << endl;
			p_stmt = cn->prepareStatement(query.c_str());
			int n_commands = 0;

			std::string typeCommand = "A";

			if (tag != "") {
				tag = "+" + tag;
			}
			p_stmt->setInt(1, commandId);
			p_stmt->setInt(2, unitId);
			if (p_stmt->execute()) {
				result = p_stmt->getResultSet();

				if (result->next()) {
					typeCommand = result->getString("type").c_str();

					if (type == 1) {
						if (typeCommand == "A") {
							n_commands = result->getInt("ap");
						}
						if (typeCommand == "W") {
							n_commands = result->getInt("wp");
						}
					} else {
						n_commands = result->getInt("qp");
						
					}
					//n_commands = result->getInt("n_commands");
					command = result->getString("command1").c_str()+tag+"="+ result->getString("password").c_str();

				}

				delete result;
				delete p_stmt;
				//delete stmt;
				if (debug) {
					//printClients();
				}

			}

			//cout << " n Commands " << n_commands << endl;
			str = command;
			if (params.size() == n_commands) {
				for (std::list<std::string>::iterator it = params.begin(); it != params.end(); ++it) {
					str = str + "," + *it;
				}

				
			}

			
			if (typeCommand == "A" && type == 2) {
				str = str + ",?";
			}
			
			printf("" ANSI_COLOR_CYAN);
			std::cout << "Command: " << str << endl;
			printf("" ANSI_COLOR_RESET);

		} catch (sql::SQLException& e) {
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			//cout << ", SQLState: " << e.getSQLState() << " )" << endl;
			//return "";
		}



		return str;
	}

	std::string DB::loadCommand(CMDMsg* msg, unsigned int historyId) {
		std::string str = "";
		
		std::string query = 
			R"(SELECT protocol_pre, c.command, d.password, coalesce(h2.value, '') as value, h.description

			FROM devices_commands as c 
			INNER JOIN devices_comm_params as p ON p.command_id = c.id
			INNER JOIN devices_versions as v ON v.id = c.version_id
			INNER JOIN devices as d ON d.version_id = v.id
			INNER JOIN units as u ON u.device_id = d.id
			INNER JOIN h_commands as h ON h.command_id = c.id and h.unit_id = u.id
			LEFT JOIN h_commands_values as h2 ON h2.h_command_id = h.id AND h2.param_id = p.id
			WHERE h.id = ')"+to_string(historyId)+"' ORDER BY `order`";

		//std::cout << query << endl << endl;

		try {
			sql::Statement* stmt;
			sql::ResultSet* result;
			sql::PreparedStatement* p_stmt;

			std::string protocol_pre = "";
			std::string command = "";
			std::string password = "";
			std::string description = "";

			stmt = cn->createStatement();
			
			p_stmt = cn->prepareStatement(query.c_str());
			
			if (p_stmt->execute()) {
				result = p_stmt->getResultSet();

				while (result->next()) {
					protocol_pre = result->getString("protocol_pre").c_str();
					command = result->getString("command").c_str();
					
					password = result->getString("password").c_str();
					description = result->getString("description").c_str();

					if (str != "") {

						str = str + "," + result->getString("value").c_str();
					} else {
						str = str + result->getString("value").c_str();
					}

				}

				delete result;
				delete p_stmt;
				delete stmt;
				if (debug) {
					//printClients();
				}

			}
			
			str = protocol_pre + command + "=" + password + "," + str;
			printf("" ANSI_COLOR_CYAN);
			std::cout << "History Command: " << description << ", " << str << endl;
			printf("" ANSI_COLOR_RESET);


		} catch (sql::SQLException& e) {
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			//cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		}



		return str;


		
	}

	InfoClient DB::getInfoClient(string id) {
		int unit_id = 0, version_id = 0, device_id = 0;

		try {
			sql::Statement* stmt;
			sql::ResultSet* result;
			sql::PreparedStatement* p_stmt;

			stmt = cn->createStatement();

			string query = "SELECT u.id as unit_id, d.id as device_id, device_name, version_id "
				"FROM units as u "
				"INNER JOIN devices as d on d.id = u.device_id "
				"WHERE device_name = '"  + id + "'";
			cout << query << endl;
			p_stmt = cn->prepareStatement(query.c_str());

			if (p_stmt->execute()) {
				result = p_stmt->getResultSet();

				while (result->next()) {
					unit_id = result->getInt("unit_id");
					device_id = result->getInt("device_id");
					version_id = result->getInt("version_id");
				}

				delete result;
				delete p_stmt;
				delete stmt;
				if (debug) {
					printClients();
				}
				//InfoClient nn = { unit_id, device_id, version_id };
				//return nn;

			}


		} catch (sql::SQLException & e) {
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			//cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		}


		InfoClient nn = { unit_id, device_id, version_id };
		return nn;
	}

	void DB::deviceConfig(const char* unit_id, CommandResult* commandResult) {
		std::string paramsList[20];
		int length = 0;
		Tool::getItem(paramsList, length, commandResult->params.c_str());

		int unitId = mClients[unit_id].unit_id;

		std::string query = R"(DELETE dc
			FROM devices_config as dc
			INNER JOIN units as u ON u.id = dc.unit_id
			INNER JOIN devices as d ON d.id = u.device_id
			INNER JOIN devices_versions as v ON v.id = d.version_id
			INNER JOIN devices_comm_params as p ON p.id = dc.param_id

			INNER JOIN devices_commands as c ON c.id = p.command_id

			WHERE c.command = ? and u.id = ?)";

		try {
			sql::Statement* stmt;
			sql::ResultSet* result;
			sql::PreparedStatement* p_stmt;

			//stmt = cn->createStatement();

			p_stmt = cn->prepareStatement(query.c_str());

			p_stmt->setString(1, commandResult->command.c_str());
			p_stmt->setInt(2, unitId);
			if (p_stmt->execute()) {
				
				delete p_stmt;
				//delete stmt;
				if (debug) {
					//printClients();
				}
			}
		} catch (sql::SQLException& e) {

			cout << endl << endl << "# ERR: SQLException in " << __FILE__;
			cout << endl << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << endl << "# ERR: " << e.what();
			cout << endl << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		}


		query = R"(SELECT
			p.id as param_id, param, u.id as unit_id, c.command


			FROM devices_commands as c
			INNER JOIN devices_comm_params as p ON p.command_id = c.id
			INNER JOIN devices_versions as v ON v.id = c.version_id
			INNER JOIN devices as d ON d.version_id = v.id
			INNER JOIN units as u ON u.device_id = d.id

			WHERE c.command = ? and u.id = ?
			ORDER BY p.order)";


		try {
			sql::Statement* stmt;
			sql::ResultSet* result;
			sql::PreparedStatement* p_stmt;

			std::string param_id = "";
			std::string str = "(";
			std::string strFields = " (`unit_id`,`device_id`,`param_id`, `value`, `update`) ";


			stmt = cn->createStatement();

			p_stmt = cn->prepareStatement(query.c_str());


			p_stmt->setString(1, commandResult->command.c_str());
			p_stmt->setInt(2, unitId);
			int x = 0;
			if (p_stmt->execute()) {


				


				result = p_stmt->getResultSet();

				while (result->next()) {
					param_id = result->getString("param_id").c_str();

					str = "(";
					if (str != "") {

						str = str + to_string(unitId) + ",2," + param_id + ",'" + paramsList[x++]+"', now()";
					} else {
						str = str + param_id;
					}
					str += ")";

					str = "INSERT INTO `devices_config` " + strFields + " VALUES " + str;
					cout << str << endl;

					save(str);

				}

				
				delete result;
				delete p_stmt;
				delete stmt;
				if (debug) {
					//printClients();
				}


				


			}
			printf("" ANSI_COLOR_GREEN);
			cout << commandResult->command << " " << commandResult->token << " " << commandResult->params << endl;
			printf("" ANSI_COLOR_RESET);
		} catch (sql::SQLException& e) {

			
			cout << endl << endl << "# ERR: SQLException in " << __FILE__;
			cout << endl << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << endl << "# ERR: " << e.what();
			cout << endl << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
			

		}
	}

	void DB::evalPending(const char* unit_id, CommandResult* commandResult) {
		

		int unitId = mClients[unit_id].unit_id;

		std::string query = R"(DELETE p
			FROM pending as p
			INNER JOIN units as u ON u.id = p.unit_id
			INNER JOIN devices as d ON d.id = u.device_id
			INNER JOIN devices_versions as v ON v.id = d.version_id


			INNER JOIN devices_commands as c ON c.id = p.command_id

			WHERE u.id = ? AND c.command = ? AND p.index = ?)";

		try {
			sql::Statement* stmt;
			
			sql::PreparedStatement* p_stmt;

			//stmt = cn->createStatement();

			p_stmt = cn->prepareStatement(query.c_str());

			p_stmt->setInt(1, unitId);
			p_stmt->setString(2, commandResult->command.c_str());
			p_stmt->setString(3, commandResult->tag.c_str());
			
			if (p_stmt->execute()) {

				delete p_stmt;
				//delete stmt;
				if (debug) {
					//printClients();
				}
			}
		} catch (sql::SQLException& e) {

			cout << endl << endl << "# ERR: SQLException in " << __FILE__;
			cout << endl << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			//cout << endl << "# ERR: " << e.what();
			cout << endl << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		}


		
	}


	void DB::save(std::string query) {

		printf("" ANSI_COLOR_CYAN);

		cout << query << endl;

		printf("" ANSI_COLOR_RESET);

		try {
			sql::Statement* stmt;
			sql::ResultSet* res;
			stmt = cn->createStatement();
			res = stmt->executeQuery(query.c_str());
			delete res;
			delete stmt;

		} catch (sql::SQLException& e) {

			if (1 == 0) {
				cout << endl << endl << "# ERR: SQLException in " << __FILE__;
				cout << endl << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
				cout << endl << "# ERR: " << e.what();
				cout << endl << " (MySQL error code: " << e.getErrorCode();
				cout << ", SQLState: " << e.getSQLState() << " )" << endl;
			}

		}
	}

	std::string  DB::addPending(unsigned int unitId, unsigned short commandId, unsigned int tag, std::string command, std::string user) {

		std:string query = "";
		sql::PreparedStatement* p_stmt;
	

		try {
			query = "DELETE FROM pending WHERE unit_id = ? AND command_id = ?";
			p_stmt = cn->prepareStatement(query.c_str());

			p_stmt->setInt(1, unitId);
			p_stmt->setInt(2, commandId);
			p_stmt->execute();
			

		} catch (sql::SQLException& e) {

		}

		query = "INSERT INTO pending (`unit_id`, `command_id`, `command`, `tag`, `index`, `user`) VALUES (?,?,?,?,?,?)";
		
		
		try {
			
			p_stmt = cn->prepareStatement(query.c_str());

			p_stmt->setInt(1, unitId);
			p_stmt->setInt(2, commandId);
			p_stmt->setString(3, command.c_str());
			p_stmt->setString(4, to_string(tag).c_str());
			p_stmt->setInt(5, tag);
			p_stmt->setString(6, user.c_str());

			
			if (p_stmt->execute()) {
			}
			//delete res;
			delete p_stmt;

		} catch (sql::SQLException& e) {

			if (1 == 0) {
				cout << endl << endl << "# ERR: SQLException in " << __FILE__;
				cout << endl << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
				cout << endl << "# ERR: " << e.what();
				cout << endl << " (MySQL error code: " << e.getErrorCode();
				cout << ", SQLState: " << e.getSQLState() << " )" << endl;
			}

		}
		return std::to_string(0);
	}

	unsigned int DB::getTag(unsigned int unitId, unsigned short commandId) {
	
		
		sql::PreparedStatement* p_stmt;
		sql::ResultSet* result = nullptr;
		unsigned short index = 1;
		std:string query = R"(
			SELECT (COALESCE(MAX(`index`) , 0) % 65535 + 1) as n
			FROM pending
			WHERE unit_id = ? AND command_id = ?
		)";

		try {
			p_stmt = cn->prepareStatement(query.c_str());

			p_stmt->setInt(1, unitId);
			p_stmt->setInt(2, commandId);

			if (p_stmt->execute()) {

				result = p_stmt->getResultSet();

				if (result->next()) {

					index = result->getInt("n");

				}
				delete result;
			}
			
			delete p_stmt;

		} catch (sql::SQLException& e) {

		}
		return index;
	}

	


}