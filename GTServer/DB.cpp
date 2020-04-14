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
				"SELECT id, tag_length, pass_default, protocol_pre,sync_header, format_id "
				"FROM devices_versions as d "
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
			//cout << ", SQLState: " << e.getSQLState() << " )" << endl;
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
				"SELECT u.id as unit_id, d.id as device_id, device_name, version_id "
				"FROM units as u "
				"INNER JOIN devices as d on d.id = u.device_id "
				"WHERE device_name IS NOT NULL "
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


		std::cout << "unit_id : " << unit_id << " version(Format) " << version  << endl;
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
		}
		query = query + "(" + qfields + ") VALUES (" + qvalues + ");";

		std::cout << query << endl;


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
		cout << msg->params << endl;
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
			cout << "query: " << query << endl;
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

			cout << " n Commands " << n_commands << endl;
			str = command;
			if (ii == n_commands) {
				for (SizeType x = 0; x < ii; x++) {
					if (str != "") {
						str = str + "," + document[x].GetString();
					}


					//cout << x << " ==> " << str << endl;
				}
			}


		} catch (sql::SQLException & e) {
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


}