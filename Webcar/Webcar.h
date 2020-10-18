#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filereadstream.h"

#include <mysql/jdbc.h>
#include <vector>
#include <sstream>
#include <cstdio>
#include <iostream>

#include <sstream>
#include <math.h>

std::string toBin(int n);
std::string toBinR(int n);
std::vector<std::string> explode(std::string const& s, char delim);

namespace WC {
	struct InfoDB {

		const char* host;
		const char* port;
		const char* name;
		const char* user;
		const char* pass;
		bool debug;

	};

	struct AppConfig {
		const char* appname;
		unsigned int port;
		unsigned int max_clients;
		const char* version;
		bool debug;
		bool show_cache;
		InfoDB db;
	};


	struct InfoDevice {
		int codequipo;
		std::string parametros;
		std::vector<std::string> params;

	};
	struct TrackParam {
		int trackId;
		int codequipo;
		std::string idEquipo;
		std::string fechaHora;
		float longitud;
		float latitud;
		int velocidad;
		int input;
	};
	struct InfoEvent {

		int codalarma;
		int codequipo;
		std::string fechaHora;
		int trackId;
		short activo;
		short status;
		short velocidad;
		short geocerca;
		short input;
		short sitio;

		std::string alarma;
		std::string descripcion;
	};

	struct InfoDBAlarm {
		int codalarma;
		int codequipo;
		int velocidad;
		short geocerca;
		short sitio;
		short status;
	};
	struct InfoParam {
		int id;
		int codalarma;
		int codequipo;
		float longitud;
		float latitud;
		int velocidad;
		int input;
		std::string idEquipo;
		std::string fechaHora;
		int trackId;
		std::string alarma;
		std::string descripcion;

		int geocerca;
		int sitio;
	};

	


	struct EventParam {
		int codequipo;
		int codalarma;
		int status;
		int velocidad;
		int geocerca;
		int sitio;
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

	struct WCPoint {
		float lat;
		float lng;
	};

	struct InfoAlarm {

		int codalarma;
		std::string alarma;
		std::string descripcion;

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
		const float R = 6378137;
		const float  PI = 3.14159265358979f;

		Webcar(InfoDB pInfo);
		Webcar(const char* path);
		~Webcar();
		bool connect(InfoDB info);
		bool test();
		//void evalTrack(int id, int codequipo, float longitud, float latitud, int velocidad, int input);
		void evalTrack(TrackParam* param);
		bool evalAlarm(InfoAlarm* alarm, TrackParam* param);

		//bool insideCircle(float pLat, float pLng, float rLat, float rLng, float dLat, float dLng);

		bool insideCircle(WCPoint* p, WCPoint* r, WCPoint* d);
		bool insideCircle2(WCPoint* p, WCPoint* r, float d);

		//bool insideCircle2(float pLat, float pLng, float rLat, float rLng, float d);
		bool insidePolygon(std::vector<WCPoint>* list, WCPoint* p);
		void getPoint(std::string str, WCPoint* point);

		void insertEvent(InfoEvent* P);
		void updateAlarm(InfoDBAlarm* P);

		InfoDevice getInfoDevice(std::string unitId);
		void insertTrack(std::string name, std::string track);
		AppConfig config;
	private:
		
		rapidjson::Document json;
		AppConfig loadConfig(const char* path);

		InfoDB info;
		const char* path;
		std::map<std::string, std::string> trackParams;

		sql::Driver* driver = nullptr;
		sql::Connection* cn = nullptr;
		sql::Statement* stmt = nullptr;
		sql::ResultSet* result = nullptr;
		sql::PreparedStatement* stmtMain;
		sql::PreparedStatement* stmtSpeed;
		sql::PreparedStatement* stmtEvent;
		sql::PreparedStatement* stmtAlarm;
		sql::PreparedStatement* stmtDevice;
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

		const char* qInsertEvent = R"(
			INSERT INTO alarma_eventos
			(codalarma, codequipo, hora, track_id, activo, status, velocidad, geocerca, input, sitio, alarma, descripcion)
			VALUES
			(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
			#($kk, $codequipo, '$fecha_hora', $id, 1, 1, $i_velocidad, $i_geocerca, $i_input, $i_sitio, '$_alarma', '$_descripcion'))";

		const char* qUpdateAlarm = R"(
			UPDATE alarma_vehiculos 
			SET status = ?, velocidad = ?,
			geocerca = ?, sitio = ?
		
			WHERE codequipo = ? AND codalarma = ?		

			)";
		const char* qDeviceInfo = R"(
			SELECT e.codequipo,v.codvehiculo,c.codigo as id_vehiculo,ce.codigo as id_equipo,
			p.parametros

			FROM vehiculos as v
			INNER JOIN codigos_vehiculos as c ON v.codigo=c.id
			INNER JOIN cuenta_vehiculos as cta ON cta.codvehiculo=v.codvehiculo
			INNER JOIN equipos as e ON e.codequipo = cta.codequipo
			INNER JOIN modelo_param as p ON p.codmodelo = version
			INNER JOIN codigos_equipos as ce ON ce.id=e.codigo_und
			WHERE ce.codigo=?)";

	};

}