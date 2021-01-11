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
			result(nullptr)
		{
		
		driver = get_driver_instance();
	}

	DB::~DB() {
		//delete stmtInfoClient;
	}
	
	bool DB::connect() {
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
	
	void DB::init() {

		if (initialized) {
			return;
		}
		
		initialized = true;

		string query = "";
		query = R"(SELECT u.id as unit_id, d.id as device_id, d.name as device_name, version_id, n.name
				FROM unit as u
				INNER JOIN device as d on d.id = u.device_id
				INNER JOIN unit_name as n ON n.id = u.name_id
				WHERE d.name = ?)";

		stmtInfoClient = cn->prepareStatement(query.c_str());

		query = R"(UPDATE unit SET conn_status=?, conn_date=? WHERE id=?)";
		stmtUpdateClientStatus = cn->prepareStatement(query.c_str());

		query = R"(SELECT n.name, p.command, d.name as device_name
			FROM pending as p
			INNER JOIN unit as u ON u.id = p.unit_id
			INNER JOIN unit_name as n ON n.id = u.name_id
			INNER JOIN device as d ON d.id = u.device_id WHERE FIND_IN_SET(p.unit_id, ?); )";
		stmtPendingCommand = cn->prepareStatement(query.c_str());
		

		stmtLoadProtocols = cn->prepareStatement(
			R"(SELECT id, tag_length, pass_default, protocol_pre,sync_header, format_id, token_ok, token_error,token_resp
				FROM device_version as d )"
		);

		stmtLoadVersions = cn->prepareStatement(
			"SELECT sync_dec FROM device_version d GROUP BY sync_dec; "
		);


		stmtCreateCommand = cn->prepareStatement(
			R"(
				SELECT count(p.id) as n_commands, c.*, CONCAT(protocol_pre, command) as command1, d.password,
					c.use_tag, n.name as unit, 
					sum(case when p.type='Q' then 1 else 0 end) as qp,
					sum(case when p.type='A' then 1 else 0 end) as ap,
					sum(case when p.type='W' then 1 else 0 end) as wp,
					sum(case when p.type='R' then 1 else 0 end) as rp 
				FROM device_command as c 
				LEFT JOIN device_comm_param as p ON p.command_id = c.id 
				INNER JOIN device_version as v ON v.id = c.version_id 
				INNER JOIN device as d ON d.version_id = v.id 
				INNER JOIN unit as u ON u.device_id = d.id 
				INNER JOIN unit_name as n ON n.id = u.name_id

				WHERE 
				c.id = ? 
				and 
				u.id = ? 
				order by c.id, `order`;)");
		

		stmtDelDeviceConfig = cn->prepareStatement(
			R"(DELETE dc
			FROM device_config as dc
			INNER JOIN unit as u ON u.id = dc.unit_id
			INNER JOIN device as d ON d.id = u.device_id
			INNER JOIN device_version as v ON v.id = d.version_id
			INNER JOIN device_comm_param as p ON p.id = dc.param_id

			INNER JOIN device_command as c ON c.id = p.command_id

			WHERE c.command = ? and u.id = ?)");


		stmtDeviceConfig = cn->prepareStatement(
			R"(SELECT
			p.id as param_id, param, u.id as unit_id, c.command


			FROM device_command as c
			INNER JOIN device_comm_param as p ON p.command_id = c.id
			INNER JOIN device_version as v ON v.id = c.version_id
			INNER JOIN device as d ON d.version_id = v.id
			INNER JOIN unit as u ON u.device_id = d.id

			WHERE c.command = ? and u.id = ?
			ORDER BY p.order)");

		stmtInsertDeviceConfig = cn->prepareStatement(
			R"(INSERT INTO `device_config` 
				(`unit_id`,`param_id`, `value`, `update`) 
				VALUES (?,?,?,now()))");

		stmtGetPending = cn->prepareStatement(
			R"(SELECT p.*
			FROM pending as p
			INNER JOIN unit as u ON u.id = p.unit_id
			INNER JOIN device as d ON d.id = u.device_id
			INNER JOIN device_version as v ON v.id = d.version_id


			INNER JOIN device_command as c ON c.id = p.command_id

			WHERE u.id = ? AND c.command = ? AND p.index = ?)");


		stmtEvalPending = cn->prepareStatement(
			R"(DELETE p
			FROM pending as p
			INNER JOIN unit as u ON u.id = p.unit_id
			INNER JOIN device as d ON d.id = u.device_id
			INNER JOIN device_version as v ON v.id = d.version_id


			INNER JOIN device_command as c ON c.id = p.command_id

			WHERE u.id = ? AND c.command = ? AND p.index = ? )");



		stmtReadCommand = cn->prepareStatement(
			R"(SELECT IF(p.type = 2, true, false) as result
			FROM pending as p
			INNER JOIN unit as u ON u.id = p.unit_id
			INNER JOIN device as d ON d.id = u.device_id
			INNER JOIN device_version as v ON v.id = d.version_id


			INNER JOIN device_command as c ON c.id = p.command_id

			WHERE u.id = ? AND c.command = ? AND p.index = ?)");


		//if (!stmtGetTag) {
			stmtGetTag = cn->prepareStatement(R"(
				SELECT (COALESCE(MAX(`index`) , 0) % 65535 + 1) as n
				FROM pending
				WHERE unit_id = ? AND command_id = ? 
			)");
		//}



		//	if (!stmtInfoCommand) {
		stmtInfoCommand = cn->prepareStatement(
			R"(SELECT p.*
			FROM pending as p
			INNER JOIN unit as u ON u.id = p.unit_id
			INNER JOIN device as d ON d.id = u.device_id
			INNER JOIN device_version as v ON v.id = d.version_id


			INNER JOIN device_command as c ON c.id = p.command_id

			WHERE u.id = ? AND c.command = ? AND p.index = ?
		)");
		//	}


		//if (!stmtSaveResponse) {
		stmtSaveResponse = cn->prepareStatement(
			R"(INSERT INTO unit_response
				(`unit_id`,`unit`,`type`,`level`,`mode`, `command_id`,`index`, `command`,`response`,`user`,`date_from`) 
				VALUES
				(?,?,?,?,?,?,?,?,?,?,IF(?='0000-00-00 00:00:00',now(),?)))");
		//}

		
		stmtDeletePending = cn->prepareStatement("DELETE FROM pending WHERE unit_id = ? AND command_id = ? ");
		stmtInsertPending = cn->prepareStatement("INSERT INTO pending (`unit_id`, `command_id`, `command`, `tag`, `index`, `user`, `type`, `mode`,`server_time`) VALUES (?,?,?,?,?,?,?,?,?)");
		
		stmtTracking = cn->createStatement();

		initStatus();
		loadProtocols();
		loadVersions();
		loadClients();
		loadFormats();
	}

	

	bool DB::isValid() {
		if (cn == NULL) {
			return false;
		}
		cn->reconnect();

		return cn->isValid();
	}

	void DB::reset() {

		
		delete stmtLoadProtocols;
		delete stmtLoadVersions;

		delete stmtCreateCommand;
		delete stmtDelDeviceConfig;
		delete stmtDeviceConfig;
		delete stmtInsertDeviceConfig;
		delete stmtGetPending;
		delete stmtEvalPending;
		delete stmtReadCommand;
		delete stmtInfoCommand;
		delete stmtSaveResponse;
		delete stmtDeletePending;
		delete stmtInsertPending;
		delete stmtGetTag;
		delete stmtInfoClient;
		delete stmtUpdateClientStatus;
		delete stmtPendingCommand;

		delete stmtTracking;


		
		//delete stmtMain;
		initialized = false;
	}

	void DB::SQLException(sql::SQLException& e, long line) {

		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << line << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		//cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}

	void DB::SQLException(sql::SQLException& e) {
		
		cout << "# ERROR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERROR - : " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode() << endl;
		//cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}

	void DB::initStatus() {

		if (!connect()) {
			return;
		}

		try {
			sql::Statement* stmt;
			
			stmt = cn->createStatement();

			stmt->execute("UPDATE unit SET conn_status = 0");

			
			delete stmt;



		} catch (sql::SQLException& e) {
			SQLException(e, __LINE__);
		}
	}

	void DB::loadProtocols() {
		
		if (!connect()) {
			return;
		}

		try {
			
			sql::ResultSet* result;

			if (stmtLoadProtocols->execute()) {
				result = stmtLoadProtocols->getResultSet();

				while (result->next()) {

					mProtocols.insert(std::pair<int, InfoProto >(result->getInt("id"), {
						result->getInt("id"),
						result->getInt("tag_length"),
						result->getString("pass_default").c_str(),
						result->getString("protocol_pre").c_str(),
						result->getString("token_ok").c_str(),
						result->getString("token_error").c_str(),
						result->getString("token_resp").c_str(),

						result->getString("sync_header").c_str(),
						result->getInt("format_id"),
						}));

				}
				
				delete result;
				
				if (debug) {
					printProtocols();
				}
				
			}


		} catch (sql::SQLException & e) {
			SQLException(e, __LINE__);
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

		if (!connect()) {
			return;
		}

		try {
			
			sql::ResultSet* result;

			if (stmtLoadVersions->execute()) {
				result = stmtLoadVersions->getResultSet();

				while (result->next()) {

					mVersions.push_back(result->getInt("sync_dec"));
					versions->e[versions->n++] = result->getInt("sync_dec");
				}

				delete result;
				
				
				if (debug) {
					printVersions();
				}

			}


		} catch (sql::SQLException & e) {
			SQLException(e, __LINE__);
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
		if (!connect()) {
			return;
		}
		try {
			sql::Statement* stmt;
			sql::ResultSet* result;
			sql::PreparedStatement* p_stmt;

			stmt = cn->createStatement();

			p_stmt = cn->prepareStatement(
				R"(
				SELECT u.id as unit_id, d.id as device_id, d.name as device_name, version_id 
				FROM unit as u 
				INNER JOIN device as d on d.id = u.device_id 
				WHERE d.name IS NOT NULL )"
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
			SQLException(e, __LINE__);
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

		if (!connect()) {
			return;
		}

		try {
			sql::Statement* stmt;
			sql::ResultSet* result;
			sql::PreparedStatement* p_stmt;

			stmt = cn->createStatement();

			p_stmt = cn->prepareStatement(
				"SELECT format_id, parameter FROM device_format as d ORDER BY format_id, `order`;"
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
			SQLException(e, __LINE__);
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

		//std::cout << "unit_id : " << unit_id << " version(Format) " << version  << endl;
		//std::cout << "buffer: " << buffer << std::endl;
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


		//cout << "*****************************" << endl;

		try {
			
			
			//printf("" ANSI_COLOR_MAGENTA);
			//std::cout << query << endl;
			//printf("" ANSI_COLOR_RESET);
			//sql::Statement* stmt;
			//sql::ResultSet* res;
			
			stmtTracking->execute(query.c_str());
			//delete res;
			//cout << ANSI_COLOR_CYAN "Saving Track: " << mClients[unit_id].device_id << endl;
			
			return true;

		} catch (sql::SQLException & e) {
			SQLException(e, __LINE__);
		}
		
		return false;
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

			//SQLException(e, __LINE__);

		}


		return true;
	}

	bool DB::saveEvent(const char* unit_id, int type_id) {

		if (!connect()) {
			return false;
		}

		printf("unit_id: %s, type: %d\n", unit_id, type_id);

		std::string query = "INSERT INTO unit_event ( unit_id, type_id, date_time) "
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

			SQLException(e, __LINE__);
			return false;

		}
		return true;
	}

	bool DB::isVersion(int value) {


		for (std::list<int>::iterator it = mVersions.begin(); it != mVersions.end(); it++) {

			
			if (value == *it) {
				//printf("is sync..%d...%d....\n", value, *it);
				return true;
			}
		}
		
		return false;
	}

	/* NO */
	std::string DB::createCommand(CMDMsg* msg, unsigned int unitId, unsigned short commandId) {
		
		if (!connect()) {
			return "";
		}

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
				"FROM device_command as c "
				"LEFT JOIN device_comm_param as p ON p.command_id = c.id "
				"INNER JOIN device_version as v ON v.id = c.version_id "
				"INNER JOIN device as d ON d.version_id = v.id "
				"INNER JOIN unit as u ON u.device_id = d.id "

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
			SQLException(e, __LINE__);
		}



		return str;
	}

	/* NO*/
	std::string DB::createCommand(unsigned int unitId, unsigned short commandId, std::string tag, std::list<string> params, unsigned short type) {
		if (!connect()) {
			return "";
		}
		cout << "Create Command" << endl;

		std::string command;
		std::string str = "";
		
		try {
			//sql::Statement* stmt;
			sql::ResultSet* result;
			sql::PreparedStatement* p_stmt;

			int useTag = 1;

			//stmt = cn->createStatement();
			string query = R"(
				SELECT count(p.id) as n_commands, c.*, CONCAT(protocol_pre, command) as command1, d.password,
					c.use_tag,
					sum(case when p.type='Q' then 1 else 0 end) as qp,
					sum(case when p.type='A' then 1 else 0 end) as ap,
					sum(case when p.type='W' then 1 else 0 end) as wp,
					sum(case when p.type='R' then 1 else 0 end) as rp 
				FROM device_command as c 
				LEFT JOIN device_comm_param as p ON p.command_id = c.id 
				INNER JOIN device_version as v ON v.id = c.version_id 
				INNER JOIN device as d ON d.version_id = v.id 
				INNER JOIN unit as u ON u.device_id = d.id 

				WHERE 
				c.id = ? 
				and 
				u.id = ? 
				order by c.id, `order`;)";
			//cout << "query: " << query << endl;
			p_stmt = cn->prepareStatement(query.c_str());
			int n_commands = 0;

			std::string typeCommand = "A";

			
			p_stmt->setInt(1, commandId);
			p_stmt->setInt(2, unitId);
			if (p_stmt->execute()) {
				result = p_stmt->getResultSet();

				if (result->next()) {
					typeCommand = result->getString("type").c_str();
					useTag = result->getInt("use_tag");
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
					cout << "super type " << type << " useTag " << useTag << endl;
					if (tag != "" && (useTag == 3 || useTag == 1 && type == 1 || useTag == 2 && type == 2)) {
						tag = "+" + tag;
					} else {
						tag = "";
					}
					
					command = result->getString("command1").c_str() + tag + "=" + result->getString("password").c_str();

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
			SQLException(e, __LINE__);
		}



		return str;
	}

	std::string DB::createCommand(RCommand* request, std::list<string> params) {
		if (!connect()) {
			return "";
		}
		unsigned int index = getTag(request->unitId, request->commandId, request->type);

		request->index = index;

		//cout << "Create Command" << endl;

		std::string command;
		std::string str = "";
		std::string tag = to_string(index);
		try {
			
			sql::ResultSet* result;
			

			int useTag = 1;

			
			string query = R"(
				SELECT count(p.id) as n_commands, c.*, CONCAT(protocol_pre, command) as command1, d.password,
					c.use_tag, n.name as unit, 
					sum(case when p.type='Q' then 1 else 0 end) as qp,
					sum(case when p.type='A' then 1 else 0 end) as ap,
					sum(case when p.type='W' then 1 else 0 end) as wp,
					sum(case when p.type='R' then 1 else 0 end) as rp 
				FROM device_command as c 
				LEFT JOIN device_comm_param as p ON p.command_id = c.id 
				INNER JOIN device_version as v ON v.id = c.version_id 
				INNER JOIN device as d ON d.version_id = v.id 
				INNER JOIN unit as u ON u.device_id = d.id 
				INNER JOIN unit_name as n ON n.id = u.name_id

				WHERE 
				c.id = ? 
				and 
				u.id = ? 
				order by c.id, `order`;)";
			//cout << "query: " << query << endl;
			
			int n_commands = 0;

			std::string typeCommand = "A";


			stmtCreateCommand->setInt(1, request->commandId);
			stmtCreateCommand->setInt(2, request->unitId);

			if (stmtCreateCommand->execute()) {
				result = stmtCreateCommand->getResultSet();

				if (result->next()) {

					strcpy_s(request->unit, sizeof(request->unit), result->getString("unit").c_str());

					typeCommand = result->getString("type").c_str();
					useTag = result->getInt("use_tag");
					if (request->type == 1) {
						if (typeCommand == "A") {
							n_commands = result->getInt("ap");
						}
						if (typeCommand == "W") {
							n_commands = result->getInt("wp");
						}
					} else {
						n_commands = result->getInt("qp");

					}
					//cout << "super type " << request->type << " useTag " << useTag << endl;
					if (tag != "" && (useTag == 3 || useTag == 1 && request->type == 1 || useTag == 2 && request->type == 2)) {
						tag = "+" + tag;
					} else {
						tag = "";
						request->index = 0;
					}

					command = result->getString("command1").c_str() + tag + "=" + result->getString("password").c_str();

				}

				delete result;

			}

			//cout << " n Commands " << n_commands << endl;
			str = command;
			if (params.size() == n_commands) {
				for (std::list<std::string>::iterator it = params.begin(); it != params.end(); ++it) {
					str = str + "," + *it;
				}
			}

			if (typeCommand == "A" && request->type == 2) {
				str = str + ",?";
			}

			//printf("" ANSI_COLOR_CYAN);
			std::cout << Color::_red() << Color::byellow() << "\nCommand: " << str << Color::_reset() << endl;
			//printf("" ANSI_COLOR_RESET);

		} catch (sql::SQLException& e) {
			SQLException(e, __LINE__);
		}

		return str;
	}

	std::string DB::loadCommand(CMDMsg* msg, unsigned int historyId) {
		if (!connect()) {
			return "";
		}
		
		std::string str = "";
		
		std::string query = 
			R"(SELECT protocol_pre, c.command, d.password, coalesce(h2.value, '') as value, h.description

			FROM device_command as c 
			INNER JOIN device_comm_param as p ON p.command_id = c.id
			INNER JOIN device_version as v ON v.id = c.version_id
			INNER JOIN device as d ON d.version_id = v.id
			INNER JOIN unit as u ON u.device_id = d.id
			INNER JOIN h_command as h ON h.command_id = c.id and h.unit_id = u.id
			LEFT JOIN h_command_value as h2 ON h2.h_command_id = h.id AND h2.param_id = p.id
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
			SQLException(e, __LINE__);
		}



		return str;


		
	}

	InfoClient DB::getInfoClient(string id) {
		if (!connect()) {
			return InfoClient();
		}
		
		int unit_id = 0, version_id = 0, device_id = 0;
		InfoClient info = {0,0,0,""};
		try {
			sql::ResultSet* result;
			
			stmtInfoClient->setString(1, id.c_str());

			if (stmtInfoClient->execute()) {
				result = stmtInfoClient->getResultSet();

				while (result->next()) {
					info.unit_id = result->getInt("unit_id");
					info.device_id = result->getInt("device_id");
					info.version_id = result->getInt("version_id");
					strcpy_s(info.name, sizeof(info.name), result->getString("name").c_str());
				}

				delete result;
			}


		} catch (sql::SQLException & e) {
			SQLException(e, __LINE__);
		}


		
		return info;
	}

	void DB::deviceConfig(const char* unit_id, CommandResult* commandResult) {
		
		if (!connect()) {
			return;
		}
		
		int unitId = mClients[unit_id].unit_id;
		std::string paramsList[20];
		int length = 0;
		Tool::getItem(paramsList, length, commandResult->params.c_str());

		std::vector<std::string> v;
		
		split2(commandResult->params.c_str(), v, ',');

		try {
			stmtDelDeviceConfig->setString(1, commandResult->command.c_str());
			stmtDelDeviceConfig->setInt(2, unitId);
			stmtDelDeviceConfig->execute();
			
		} catch (sql::SQLException& e) {

			SQLException(e, __LINE__);
		}

		try {
			
			sql::ResultSet* result;

			stmtDeviceConfig->setString(1, commandResult->command.c_str());
			stmtDeviceConfig->setInt(2, unitId);
			
			int x = 0;
			if (stmtDeviceConfig->execute()) {

				result = stmtDeviceConfig->getResultSet();

				while (result->next()) {

					if (x >= v.size()) {
						continue;
					}

					stmtInsertDeviceConfig->setInt(1, unitId);
					stmtInsertDeviceConfig->setInt(2, result->getInt("param_id"));
					stmtInsertDeviceConfig->setString(3, v[x].c_str());
					stmtInsertDeviceConfig->execute();
					
					x++;
				}
				
				delete result;
			}
			//cout << commandResult->command << " " << commandResult->token << " " << commandResult->params << endl;
			
		} catch (sql::SQLException& e) {
			SQLException(e, __LINE__);
		}
	}

	void DB::getPending(const char* unit_id, CommandResult* commandResult, RCommand* response) {
		if (!connect()) {
			return;
		}

		int unitId = mClients[unit_id].unit_id;

		response->header = 10050;
		response->unitId = unitId;
		strcpy_s(response->unit, sizeof(response->unit), unit_id);
		
		try {
			sql::ResultSet* result = nullptr;

			stmtGetPending->setInt(1, unitId);
			stmtGetPending->setString(2, commandResult->command.c_str());
			stmtGetPending->setString(3, commandResult->tag.c_str());
			
			if (stmtGetPending->execute()) {
				result = stmtGetPending->getResultSet();

				if (result->next()) {
					strcpy_s(response->message, sizeof(response->message), result->getString("command").c_str());
					strcpy_s(response->user, sizeof(response->user), result->getString("user").c_str());
					response->type = result->getInt("type");
					response->level = result->getInt("level");
					response->index = result->getInt("index");
					strcpy_s(response->date, sizeof(response->date), result->getString("datetime").c_str());
				}

				delete result;
				
			}
		} catch (sql::SQLException& e) {

			SQLException(e, __LINE__);
		}



	}

	void DB::evalPending(const char* unit_id, CommandResult* commandResult, unsigned int type) {
		if (!connect()) {
			return;
		}

		int unitId = mClients[unit_id].unit_id;
		
		try {

			stmtEvalPending->setInt(1, unitId);
			stmtEvalPending->setString(2, commandResult->command.c_str());
			stmtEvalPending->setString(3, commandResult->tag.c_str());
			//p_stmt->setInt(4, type);

			stmtEvalPending->execute();
			
		} catch (sql::SQLException& e) {

			SQLException(e, __LINE__);
		}


		
	}

	bool DB::isReadCommand(const char* unit_id, CommandResult* commandResult) {
		if (!connect()) {
			return false;
		}
		
		int unitId = mClients[unit_id].unit_id;
		
		bool isRead = false;
		

		try {
			sql::ResultSet* result = nullptr;

			stmtReadCommand->setInt(1, unitId);
			stmtReadCommand->setString(2, commandResult->command.c_str());
			stmtReadCommand->setString(3, commandResult->tag.c_str());

			if (stmtReadCommand->execute()) {

				result = stmtReadCommand->getResultSet();

				if (result->next()) {

					isRead = result->getBoolean("result");

				}
				delete result;
			}


		} catch (sql::SQLException& e) {
			SQLException(e, __LINE__);
		}
		
		return isRead;
	}

	void DB::setClientStatus(unsigned int unitId, unsigned short status, char* date) {
		if (!connect()) {
			return;
		}

		try {

			
			stmtUpdateClientStatus->setInt(1, status);
			stmtUpdateClientStatus->setString(2, date);
			stmtUpdateClientStatus->setInt(3, unitId);
			stmtUpdateClientStatus->execute();


		} catch (sql::SQLException& e) {
			SQLException(e, __LINE__);
		}
	}

	bool DB::getPendingCommand(std::vector<GT::PendingCommand>* pending, std::vector<std::string> units) {
		if (!connect()) {
			return false;
		}
		std::string str = "";
		//std::map<std::string, std::vector<string>> map;
		for (std::vector<std::string>::iterator it = units.begin(); it != units.end(); ++it) {
			if (str != "") {
				str += "," + *it;
			} else {
				str = *it;
			}
			
		}
		//std::cout << str << std::endl << "\n\n\n";
		
		
		std::string name = "";
		std::string command = "";
		
		try {
			sql::ResultSet* result = nullptr;
			stmtPendingCommand->setString(1, str.c_str());
			
			
			if (stmtPendingCommand->execute()) {
			
				result = stmtPendingCommand->getResultSet();

				while (result->next()) {
					name = result->getString("device_name").c_str();
					command = result->getString("command").c_str();
					pending->push_back({ name, command });
					//map[name].push_back(command);
					
					
					
					//mapCommand[name] = std:vpush_back(command);
					//index = result->getInt("T");

				}
				delete result;
			}

			

		} catch (sql::SQLException& e) {
			SQLException(e, __LINE__);
			return false;
		}

		return true;

	}

	void DB::test(int id) {
		if (!connect()) {
			return;
		}

		sql::PreparedStatement* p_stmt;
		sql::ResultSet* result = nullptr;
		unsigned short index = 0;
		std:string query = R"(
				SELECT count(*) as T FROM pending p 
			)";

		try {
			p_stmt = cn->prepareStatement(query.c_str());

			//p_stmt->setInt(1, id);
			
			//p_stmt->setInt(3, type);

			if (p_stmt->execute()) {

				result = p_stmt->getResultSet();

				if (result->next()) {

					index = result->getInt("T");

				}
				delete result;
			}

			delete p_stmt;

		} catch (sql::SQLException& e) {
			SQLException(e, __LINE__);
		}

		std::cout << "Mysql Test " << index << std::endl;
		return;
	}

	void DB::infoCommand(const char* unit_id, CommandResult* commandResult, RCommand * info) {
		if (!connect()) {
			return;
		}
		
		int unitId = mClients[unit_id].unit_id;
		bool isRead = false;

		info->header = 0;
		info->unitId = unitId;
		int len = strlen(info->unit);

		strcpy(info->unit, unit_id);
		
		sql::ResultSet* result = nullptr;

		try {

			
			

			stmtInfoCommand->setInt(1, unitId);
			stmtInfoCommand->setString(2, commandResult->command.c_str());
			stmtInfoCommand->setString(3, commandResult->tag.c_str());

			if (stmtInfoCommand->execute()) {

				result = stmtInfoCommand->getResultSet();

				if (result->next()) {
					info->header = (unsigned short)1;
					info->index = (unsigned short)result->getInt("index");
					info->type = (unsigned short)result->getInt("type");
					info->level = (unsigned short)result->getInt("level");
					info->mode = (unsigned short)result->getInt("mode");
					
					info->commandId = (int)result->getInt("command_id");
					//strcpy(info->message, result->getString("command").c_str());
					//strcpy(info->date, result->getString("date").c_str());
					//strcpy(info->user, result->getString("user").c_str());
					
					strcpy_s(info->message, sizeof(info->message), result->getString("command").c_str());
					strcpy_s(info->date, sizeof(info->date), result->getString("datetime").c_str());
					strcpy_s(info->user, sizeof(info->user), result->getString("user").c_str());

					info->time = result->getInt64("server_time");
				}
				delete result;
			}

		} catch (sql::SQLException& e) {
			SQLException(e, __LINE__);
		}

		
	}
	
	void DB::saveResponse(RCommand* info, const char* response) {
		if (!connect()) {
			return;
		}
		
		try {
			
			//cout << ANSI_COLOR_YELLOW "unit name " << info->unit << endl;
			//cout << ANSI_COLOR_MAGENTA "reponse " << response << endl;
			stmtSaveResponse->setInt(1, info->unitId);
			stmtSaveResponse->setString(2, info->unit);
			stmtSaveResponse->setInt(3, info->type);
			stmtSaveResponse->setInt(4, info->level);
			stmtSaveResponse->setInt(5, info->mode);
			stmtSaveResponse->setInt(6, info->commandId);
			stmtSaveResponse->setInt(7, info->index);
			stmtSaveResponse->setString(8, info->message);
			stmtSaveResponse->setString(9, response);
			stmtSaveResponse->setString(10, info->user);
			stmtSaveResponse->setString(11, info->date);
			stmtSaveResponse->setString(12, info->date);

			cout << ANSI_COLOR_MAGENTA "info->unitId " << info->unitId << endl;
			cout << ANSI_COLOR_MAGENTA "info->unit " << info->unit << endl;
			cout << ANSI_COLOR_MAGENTA "info->type " << info->type << endl;
			cout << ANSI_COLOR_MAGENTA "info->level " << info->level << endl;
			cout << ANSI_COLOR_MAGENTA "info->mode " << info->mode << endl;
			cout << ANSI_COLOR_MAGENTA "info->commandId " << info->commandId << endl;
			cout << ANSI_COLOR_MAGENTA "info->index " << info->index << endl;

			cout << ANSI_COLOR_MAGENTA "info->message " << info->message << endl;
			cout << ANSI_COLOR_MAGENTA "response " << response << endl;
			cout << ANSI_COLOR_MAGENTA "info->user " << info->user << endl;
			cout << ANSI_COLOR_MAGENTA "info->date " << info->date << endl;
			
			
			stmtSaveResponse->execute();

		} catch (sql::SQLException& e) {
			SQLException(e, __LINE__);
		}

	}
	
	void DB::save(std::string query) {
		if (!connect()) {
			return;
		}
		//printf("" ANSI_COLOR_CYAN);

		//cout << query << endl;

		//printf("" ANSI_COLOR_RESET);

		try {
			sql::Statement* stmt;
			sql::ResultSet* res;
			stmt = cn->createStatement();
			res = stmt->executeQuery(query.c_str());
			delete res;
			delete stmt;

		} catch (sql::SQLException& e) {

			//SQLException(e, __LINE__);

		}
	}

	std::string  DB::addPending(unsigned int unitId, unsigned short commandId, unsigned int tag, std::string command, std::string user, unsigned short type, unsigned short level) {
		if (!connect()) {
			return "";
		}
		std:string query = "";
		sql::PreparedStatement* p_stmt;
	

		try {
			query = "DELETE FROM pending WHERE `unit_id` = ? AND `command_id` = ? ";
			p_stmt = cn->prepareStatement(query.c_str());
			
			p_stmt->setInt(1, unitId);
			p_stmt->setInt(2, commandId);
			//p_stmt->setInt(3, type);
			p_stmt->execute();
			

		} catch (sql::SQLException& e) {
			SQLException(e, __LINE__);
		}

		query = "INSERT INTO pending (`unit_id`, `command_id`, `command`, `tag`, `index`, `user`, `type`) VALUES (?,?,?,?,?,?,?)";
		
		
		try {
			
			p_stmt = cn->prepareStatement(query.c_str());

			p_stmt->setInt(1, unitId);
			p_stmt->setInt(2, commandId);
			p_stmt->setString(3, command.c_str());
			p_stmt->setString(4, to_string(tag).c_str());
			p_stmt->setInt(5, tag);
			p_stmt->setString(6, user.c_str());
			p_stmt->setInt(7, type);

			if (p_stmt->execute()) {
			}
			//delete res;
			delete p_stmt;

		} catch (sql::SQLException& e) {

			SQLException(e, __LINE__);
			

		}
		return std::to_string(0);
	}
	
	std::string  DB::addPending(RCommand * request) {
		if (!connect()) {
			return "";
		}
		std:string query = "";
		//sql::PreparedStatement* p_stmt;


		try {

			

			stmtDeletePending->setInt(1, request->unitId);
			stmtDeletePending->setInt(2, request->commandId);
			//p_stmt->setInt(3, request->type);
			stmtDeletePending->execute();


		} catch (sql::SQLException& e) {
			SQLException(e, __LINE__);
		}


		
		

		//cout << "creando pending " << query << endl;
		try {

			

			stmtInsertPending->setInt(1, request->unitId);
			stmtInsertPending->setInt(2, request->commandId);
			stmtInsertPending->setString(3, request->message);
			stmtInsertPending->setString(4, to_string(request->index).c_str());
			stmtInsertPending->setInt(5, request->index);
			stmtInsertPending->setString(6, request->user);
			stmtInsertPending->setInt(7, request->type);
			stmtInsertPending->setInt(8, request->mode);

			time_t now;
			time(&now);  /* get current time; same as: now = time(NULL)  */

			stmtInsertPending->setInt(9, now);
			stmtInsertPending->execute();

		} catch (sql::SQLException& e) {
			SQLException(e, __LINE__);
		}

		return std::to_string(0);
	}

	
	
	unsigned int DB::getTag(unsigned int unitId, unsigned short commandId, unsigned int type) {
		
		if (!connect()) {
			return 0;
		}
		
		unsigned short index = 1;

		try {
			sql::ResultSet* result = nullptr;

			
			
			stmtGetTag->setInt(1, unitId);
			stmtGetTag->setInt(2, commandId);
			//p_stmt->setInt(3, type);

			if (stmtGetTag->execute()) {

				result = stmtGetTag->getResultSet();

				if (result->next()) {
					index = result->getInt("n");
				}
				delete result;
			}
			
		} catch (sql::SQLException& e) {
			SQLException(e, __LINE__);
		}

		return index;
	}

	


}