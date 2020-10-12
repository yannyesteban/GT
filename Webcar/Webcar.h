#pragma once
#include <mysql/jdbc.h>
std::string toBin(int n);
std::string toBinR(int n);



struct InfoDB {

	const char* host;
	const char* port;
	const char* name;
	const char* user;
	const char* pass;
	bool debug;

};

struct InfoSite {
	double longitud;
	double latitud;
	int radio;
	int modo;
};
struct InfoGeo {
	int t;
	std::string c;
	int v;
};



struct InfoAlarm {
	int velMin;
	int velMax;
	int lastVel;
	int lastGeo;
	int lastSit;
	int geoModo;
	int inputModo;
	int sitioModo;
	int sitioRadio;

	std::string fDesde;
	std::string fHasta;

	int ii;
	int input[8];

	std::map<int, int> inputI;

	std::map<int, InfoSite> s;
	std::map<int, InfoGeo> g;
	std::map<int, int> i;

	int st;
};

class Webcar {
public:

	Webcar(InfoDB pInfo);
	~Webcar();
	bool connect();
	bool test();
	void evalTrack(int id, int codequipo, float longitud, float latitud, int velocidad, int input);

	bool evalAlarm(InfoAlarm alarm);
private:

	InfoDB info;

	sql::Driver* driver = nullptr;
	sql::Connection* cn = nullptr;
	sql::Statement* stmt = nullptr;
	sql::ResultSet* result = nullptr;
	sql::PreparedStatement* stmtMain;
	sql::PreparedStatement* stmtSpeed;
	const char* qSpeedVar = "SELECT ? into @_vel;";
	//const char* qMain = R"(SELECT @_vel:=? as v1,@_vel+1 as z,
	const char* qMain = R"(SELECT @_vel+1 as z,
			av.codalarma,
			ag.codgeocerca,
			

			av.status as st, vel_max, vel_min,
			
			av.velocidad as last_vel,
			av.geocerca as last_geo,
			av.sitio as last_sit,
			
			a.alarma, a.descripcion,
			
			a.desde, a.hasta, 
			
			
			a.geo_modo, a.input_modo,
			ag.codgeocerca, ag.valor as gvalor,
			g.tipo as gtipo, g.coords, sitio_radio, sitio_modo,
			si.codsitio, si.latitud, si.longitud,
			
			
			/* el input que se busca*/
			ai.codinput,
			ai.codtipo, 
			
			vi.codvehiculo,
			
			
			/*los input definidos para el vehiculo*/
			vi.input_1, vi.input_on_1, vi.input_off_1,
			vi.input_2, vi.input_on_2, vi.input_off_2,
			vi.input_3, vi.input_on_3, vi.input_off_3,
			vi.input_4, vi.input_on_4, vi.input_off_4,
			vi.input_5, vi.input_on_5, vi.input_off_5,
			vi.input_6, vi.input_on_6, vi.input_off_6,
			vi.input_7, vi.input_on_7, vi.input_off_7,
			vi.input_8, vi.input_on_8, vi.input_off_8
			
		FROM alarma_vehiculos as av
		INNER JOIN alarmas as a ON a.codalarma = av.codalarma
		INNER JOIN cuenta_vehiculos as cv ON cv.codequipo = av.codequipo
		
		LEFT JOIN alarma_sitios as asi ON asi.codalarma = av.codalarma
	    LEFT JOIN sitios as si ON si.codsitio = asi.codsitio
		
		LEFT JOIN alarma_geocercas as ag ON ag.codalarma = av.codalarma
		LEFT JOIN geocercas as g ON g.codgeocerca = ag.codgeocerca
		
		LEFT JOIN alarma_inputs as ai ON ai.codalarma = av.codalarma
		
		LEFT JOIN inputs as i ON i.codinput = ai.codinput
		LEFT JOIN vehiculo_inputs as vi ON vi.codvehiculo = cv.codvehiculo

		WHERE av.codequipo = ?
		
		
		ORDER BY  av.codalarma)";
};

