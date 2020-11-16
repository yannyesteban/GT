#pragma once

#include <mysql/jdbc.h>
#include <Windows.h>

//#include <stdlib.h>
#include <cstring>
#include <cstdlib> 
//#include <stdlib.h>
#include <tchar.h>
/**/
wchar_t* convert_utf8_to_utf16(const char* utf8_ptr);


namespace S {
	class Mysql {
	public:
		Mysql();
		~Mysql();
		void test(TCHAR* message);
		bool isValid();
		
		bool init();
		void reset();
		
		bool connect();
		bool connect(TCHAR *);

		void SQLException(sql::SQLException& e);
	private:
		bool initialized = false;


		const char* qMain = R"(
			SELECT e.codequipo,v.codvehiculo,c.codigo as id_vehiculo,ce.codigo as id_equipo,
			p.parametros

			FROM vehiculos as v
			INNER JOIN codigos_vehiculos as c ON v.codigo=c.id
			INNER JOIN cuenta_vehiculos as cta ON cta.codvehiculo=v.codvehiculo
			INNER JOIN equipos as e ON e.codequipo = cta.codequipo
			INNER JOIN modelo_param as p ON p.codmodelo = version
			INNER JOIN codigos_equipos as ce ON ce.id=e.codigo_und
			WHERE ce.codigo=?)";
	protected:
		sql::Driver* driver = nullptr;
		sql::Connection* cn = nullptr;
		sql::Statement* stmt = nullptr;
		sql::ResultSet* result = nullptr;

		sql::PreparedStatement* stmtMain = nullptr;
		sql::PreparedStatement* stmtInfoClient = nullptr;
	};

}