#include "Mysql.h"

using namespace std;

S::Mysql::Mysql() {
	driver = get_driver_instance();
}

S::Mysql::~Mysql() {
	
	delete cn;
	delete stmt;
	delete result;

	sql::PreparedStatement* stmtMain = nullptr;
	sql::PreparedStatement* stmtInfoClient = nullptr;
}

bool S::Mysql::connect() {
	OutputDebugString(_T("\n\nConnect()"));
	try {
		bool reconnect = false;
		bool connected = false;



		if (cn == NULL) {
			connected = false;
		} else {
			OutputDebugString(_T("\ncn->isValid() || cn->reconnect()"));
			reconnect = (cn->isValid() || cn->reconnect());
		}

		if (reconnect) {
			reset();
			return true;
		}
		
		
		OutputDebugString(_T("\nIsValid(): "));
		//OutputDebugString((LPCWSTR)to_string(cn->isValid()).c_str());
		OutputDebugString(_T("\n..."));
		if (!connected) {

			//reconnect = (cn->isValid() || cn->reconnect());

			char str_host[100] = "tcp://";
			strcat_s(str_host, "localhost");
			strcat_s(str_host, ":");
			strcat_s(str_host, "3306");

			cn = driver->connect(str_host, "root", "123456");
			connected = cn->isValid();
			bool myTrue = true;
			cn->setClientOption("OPT_RECONNECT", &myTrue);
			cn->setSchema("cota");

			reset();
			OutputDebugString(_T("Mysql Conectado"));

		}

		if (connected) {
			
			cout << "Mysql has connected correctaly " << cn->isValid() << endl;
			/* Connect to the MySQL test database */

			OutputDebugString(_T("Mysql Todo bien"));
			return true;
		}
		
		
		

	} catch (sql::SQLException& e) {
		OutputDebugString(_T("Mysql Error!!"));
		SQLException(e);
		
	}
	return false;
}

bool S::Mysql::init() {
	OutputDebugString(_T("\ninit()"));
	
	if (!initialized) {
		stmtMain = cn->prepareStatement(qMain);
		initialized = true;
	}
	
	
	return true;
}

void S::Mysql::reset() {


	OutputDebugString(_T("\Reset()"));



	delete stmtMain;
	initialized = false;
	
}

bool S::Mysql::connect(TCHAR* message) {

	if (connect()) {
		wcscpy_s(message, 150, _T("Mysql has connected correctaly !!!"));
		return true;
	} else {
		wcscpy_s(message, 150, L"Mysql is dead! ");//_tcslen(message)
		//strcpy_s(message, sizeof(message), "Mysql is dead! ");
	}

	return false;
	
}

void S::Mysql::SQLException(sql::SQLException& e) {
	OutputDebugString(_T("Mysql SQLException!!"));
	cout << "# ERR: SQLException in " << __FILE__;
	cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
	cout << "# ERR: " << e.what();
	cout << " (MySQL error code: " << e.getErrorCode();
	//cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	
}

void S::Mysql::test(TCHAR* message) {
	/*
	InfoParam P{
		id, codequipo, longitud, latitud, velocidad, input
	};
	*/
	
	//bool isRead = false;
	if (!connect()) {
		OutputDebugString(_T("Reconnecting!!"));
		wcscpy_s(message, 150, _T("Reconnecting"));
		return;
	}
	init();
	try {
		
		
		//sql::PreparedStatement* p_stmt;
		sql::ResultSet* result = nullptr;

		//stmtMain->setInt(1, velocidad);
		stmtMain->setInt(1, 1012000561);
		
		OutputDebugString(_T("\n\nLeyendo Datos\n"));
		if (stmtMain->execute()) {
			wcscpy_s(message, 150, _T("Test 2"));
			
			result = stmtMain->getResultSet();

			int codalarma;
			int codsitio;
			int codgeocerca;
			int codtipo;
			while (result->next()) {

				codalarma = result->getInt("codvehiculo");
				OutputDebugString(_T("\n\n nada"));
				OutputDebugStringA((const char *)result->getString("id_vehiculo").c_str());
				std::string  h = result->getString("id_vehiculo").c_str();
				wcscpy_s(message, 150, (LPCWSTR)h.c_str());
				
				int i = 0;

			}
			delete result;
		}


	
		//delete p_stmt;

	} catch (sql::SQLException& e) {
		SQLException(e);
	}


}

bool S::Mysql::isValid() {
	if (cn == NULL) {
		return false;
	}
	cn->reconnect();

	return cn->isValid();
}
