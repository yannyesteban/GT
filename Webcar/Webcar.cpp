#include "Webcar.h"

using namespace std;

bool isNumber(std::string ss) {

	std::smatch m;
	
	std::regex re("^-?[0-9][0-9,\\.]+$");

	if (std::regex_search(ss, m, re)) {
		return true;
	}
	return false;

}
std::string alphaNumeric(std::string ss) {
	
	std::smatch m;
	//std::string ss("\xFA\xF8\xE7\x03\xB1yanny nunez,4.5,-8.369898,esteban");

	std::regex Pala("([a-zA-Z0-9\\-\\,\\.\\s]+)");

	while (std::regex_search(ss, m, Pala)) {
		for (int i = 0; i < m.size(); i++) {
			std::cout << " *** " << m[i].str() << "\n";
			return m[i].str();
		}

		ss = m.suffix().str();
	}
	return "";
}
std::string toBin(int n) {
	std::string r;
	while (n != 0) {
		r = (n % 2 == 0 ? "0" : "1") + r;
		n /= 2;
	}
	return r;
}

std::string toBinR(int n) {
	std::string r;
	while (n != 0) {
		r += (n % 2 == 0 ? "0" : "1");
		n /= 2;
	}
	return r;
}

std::vector<std::string> getItem(const char* buffer) {
	std::vector<string> result;
	std::smatch m;
	std::string ss(buffer);
	
	std::regex Pala("[^,]+");
	
	while (std::regex_search(ss, m, Pala)) {
		for (int i = 0; i < m.size(); i++) {
			result.push_back(m[i].str());
		}

		ss = m.suffix().str();
	}
	return result;
}

std::vector<std::string> explode(std::string const& s, char delim) {
	std::vector<std::string> result;
	std::istringstream iss(s);

	for (std::string token; std::getline(iss, token, delim); )
	{
		result.push_back(std::move(token));
	}

	return result;
}

std::vector<std::string> split(const std::string& s, char delim) {
	vector<std::string> result;
	std::stringstream ss(s);
	std::string item;

	while (getline(ss, item, delim)) {
		result.push_back(item);
	}

	return result;
}

namespace WC {
	Webcar::Webcar(InfoDB pInfo) :
		path(""),
		info(pInfo),
		driver(nullptr),
		cn(nullptr),
		stmt(nullptr),
		stmtMain(nullptr),
		stmtEvent(nullptr),
		stmtSpeed(nullptr),
		stmtAlarm(nullptr),
		stmtDevice(nullptr),
		result(nullptr) {
	}

	Webcar::Webcar(const char* pPath) :
		path(pPath),
		info({}),
		driver(nullptr),
		cn(nullptr),
		stmt(nullptr),
		stmtMain(nullptr),
		stmtEvent(nullptr),
		stmtSpeed(nullptr),
		stmtAlarm(nullptr),
		stmtDevice(nullptr),
		result(nullptr)
	{
		loadConfig(pPath);
		connect(config.db);
	}
	
	Webcar::~Webcar() {
		delete stmtMain;
		delete stmtSpeed;
		delete stmtEvent;
		delete stmtAlarm;
		delete stmtDevice;
		delete stmtLastId;
		delete stmtTrack;
		connect(config.db);
	}

	void Webcar::reconnect() {
		if (stmtMain) {
			delete stmtMain;
		}
		if (stmtSpeed) {
			delete stmtSpeed;
		}
		if (stmtEvent) {
			delete stmtEvent;
		}
		if (stmtAlarm) {
			delete stmtAlarm;
		}
		if (stmtDevice) {
			delete stmtDevice;
		}
		if (stmtLastId) {
			delete stmtLastId;
		}
		if (stmtTrack) {
			delete stmtTrack;
		}

		driver = nullptr;
		cn = nullptr;
		stmt = nullptr;
		result = nullptr;
		stmtMain = nullptr;
		stmtSpeed = nullptr;
		stmtEvent = nullptr;
		stmtAlarm = nullptr;
		stmtDevice = nullptr;
		stmtLastId = nullptr;
		connect(config.db);
		
	}

	bool Webcar::connect(InfoDB info) {
		try {
			driver = get_driver_instance();

			char str_host[100] = "tcp://";
			strcat_s(str_host, info.host);
			strcat_s(str_host, ":");
			strcat_s(str_host, info.port);

			cn = driver->connect(str_host, info.user, info.pass);
			//cout << "Mysql has connected correctaly " << cn->isValid() << endl;
			//cout << "DB: " << info.name << endl;
			/* Connect to the MySQL test database */
			cn->setSchema(info.name);
			cout << "Mysql has connected correctaly, db: " << info.name << endl;
			stmtMain = cn->prepareStatement(qMain);
			stmtSpeed = cn->prepareStatement(qSpeedVar);
			stmtEvent = cn->prepareStatement(qInsertEvent);
			stmtAlarm = cn->prepareStatement(qUpdateAlarm);
			stmtDevice = cn->prepareStatement(qDeviceInfo);
			stmtLastId = cn->prepareStatement(qLastId);

			return 1;

		} catch (sql::SQLException& e) {
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			//cout << ", SQLState: " << e.getSQLState() << " )" << endl;
			return 0;
		}
	}

	bool Webcar::test() {
		return false;
	}

	//void Webcar::evalTrack(int id, int codequipo, float longitud, float latitud, int velocidad, int input) {
	void Webcar::evalTrack(TrackParam* P) {
		/*
		InfoParam P{
			id, codequipo, longitud, latitud, velocidad, input
		};
		*/
		stmtSpeed->setInt(1, P->velocidad);
		if (!stmtSpeed->execute()) {
			//cout << "todo mal" << endl;
		}

		//sql::PreparedStatement* p_stmt;
		sql::ResultSet* result = nullptr;
		//bool isRead = false;


		try {


			//stmtMain->setInt(1, velocidad);
			stmtMain->setInt(1, P->codequipo);

			//cout << "stage zero " << P->codequipo << endl;

			std::map<int, InfoAlarm> info;

			std::string inputI;
			std::string inputOn;

			std::string inputOff;
			if (stmtMain->execute()) {
				string _input = toBinR(P->input);
				std::string::iterator it;
				int i = 0;
				char xx[8];
				result = stmtMain->getResultSet();

				int codalarma;
				int codsitio;
				int codgeocerca;
				int codtipo;
				while (result->next()) {
					codalarma = result->getInt("codalarma");
					codsitio = result->getInt("codsitio");
					codgeocerca = result->getInt("codgeocerca");
					codtipo = result->getInt("codtipo");

					info[codalarma].codalarma = codalarma;
					info[codalarma].alarma = result->getString("alarma").c_str();
					info[codalarma].descripcion = result->getString("descripcion").c_str();

					info[codalarma].velMin = result->getInt("vel_min");
					info[codalarma].velMax = result->getInt("vel_max");

					info[codalarma].lastVel = result->getInt("last_vel");
					info[codalarma].lastGeo = result->getInt("last_geo");
					info[codalarma].lastSit = result->getInt("last_sit");

					info[codalarma].geoModo = result->getInt("geo_modo");
					info[codalarma].inputModo = result->getInt("input_modo");

					info[codalarma].sitioModo = result->getInt("sitio_modo");
					info[codalarma].sitioRadio = result->getInt("sitio_radio");

					info[codalarma].fDesde = result->getString("desde").c_str();
					info[codalarma].fHasta = result->getString("hasta").c_str();

					//info[codalarma].ii = result->getInt("input");

					it = _input.begin();
					for (i = 0; i < 8; i++) {
						if (it != _input.end()) {
							xx[i] = *it;
							++it;
						} else {
							xx[i] = '0';
						}
						inputI = "input_" + to_string(i + 1);

						if (result->getInt(inputI.c_str())) {

							inputOn = "input_on_" + to_string(i + 1);
							inputOff = "input_off_" + to_string(i + 1);
							info[codalarma].inputI[result->getInt(inputI.c_str())] = (xx[i] == '1') ? result->getInt(inputOn.c_str()) : result->getInt(inputOff.c_str());
						}

					}

					if (codsitio) {
						info[codalarma].s[codsitio].longitud = result->getDouble("longitud");
						info[codalarma].s[codsitio].latitud = result->getDouble("latitud");
						info[codalarma].s[codsitio].radio = result->getInt("sitio_radio");
						info[codalarma].s[codsitio].modo = result->getInt("sitio_modo");
					}

					if (codgeocerca) {
						info[codalarma].g[codgeocerca].t = result->getInt("gtipo");
						info[codalarma].g[codgeocerca].c = result->getString("coords").c_str();
						info[codalarma].g[codgeocerca].v = result->getInt("gvalor");
					}

					if (codtipo) {
						info[codalarma].i[codtipo] = result->getInt("codinput");
					}
					info[codalarma].st = result->getInt("st");
					//cout << "vel max " << info[codalarma].velMax << endl;

				}
				delete result;
			}


			for (std::map<int, InfoAlarm>::iterator it = info.begin(); it != info.end(); ++it) {
				evalAlarm(&it->second, P);
			}
			//delete p_stmt;

		} catch (sql::SQLException& e) {
			cout << endl << endl << "# ERR: SQLException in " << __FILE__;
			cout << endl << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << endl << "# ERR: " << e.what();
			cout << endl << " (MySQL error code: " << e.getErrorCode();
		}


	}

	bool Webcar::evalAlarm(InfoAlarm* A, TrackParam* P) {

		short _min = -1;
		short _max = -1;
		short _geo = -1;
		short _inp = -1;
		short _sit = -1;

		short _geoIn = -1;
		short _sitIn = -1;

		if (A->velMin) {
			_min = 0;
			if (P->velocidad < A->velMin && P->velocidad < A->lastVel) {
				_min = 1;
			}
		}

		if (A->velMax) {
			_max = 0;
			if (P->velocidad > A->velMax && P->velocidad > A->lastVel) {
				_max = 1;
			}
		}


		if (A->g.size() > 0) {
			_geoIn = 0;
			short mode = 0;
			WCPoint latLng = { P->latitud, P->longitud };
			WCPoint center;
			WCPoint radio;

			std::vector<std::string> aux;
			string coords;
			short type;
			bool value = false;
			for (std::map<int, InfoGeo>::iterator it = A->g.begin(); it != A->g.end(); ++it) {
				coords = it->second.c;
				type = it->second.t;
				mode = it->second.v;
				value = false;

				switch (type) {
				case 1:

					aux = explode(coords, ',');
					getPoint(aux[0], &center);
					getPoint(aux[1], &radio);

					value = insideCircle(&latLng, &center, &radio);
					//std::vector
					break;
				case 2:

					std::vector<WCPoint> list;
					aux = explode(coords, ',');
					WCPoint point;
					for (std::vector<std::string>::iterator it = aux.begin(); it != aux.end(); ++it) {
						getPoint(*it, &point);
						list.push_back(point);

					}
					value = insidePolygon(&list, &latLng);

					break;
				}

				if (value) {
					_geoIn = 1;
					break;
				}
			}

			if (_geoIn == 1) {
				_geo = 0;
				switch (mode) {
				case 1:
					_geo = 1;
					break;
				case 2:
					break;
				case 3:
					if (A->lastGeo == 0) {
						_geo = 1;
					}
					break;
				case 4:
					break;
				}

			} else {
				_geo = 0;
				switch (mode) {
				case 1:

					break;
				case 2:
					_geo = 1;
					break;
				case 3:
					break;
				case 4:
					if (A->lastGeo == 1) {
						_geo = 1;
					}
					break;
				}
			}



		}


		if (A->s.size() > 0) {
			_sitIn = 0;
			short mode = 0;
			_sit = 0;
			WCPoint latLng = { P->latitud, P->longitud };
			WCPoint center;
			int distance;
			bool value = false;
			for (std::map<int, InfoSite>::iterator it = A->s.begin(); it != A->s.end(); ++it) {

				mode = it->second.modo;
				distance = it->second.radio;
				center.lat = it->second.latitud;
				center.lng = it->second.longitud;

				value = insideCircle2(&latLng, &center, distance);

				if (value == 1) {
					_sitIn = 1;
					break;
				}

			}

			if (_sitIn == 1) {

				switch (mode) {
				case 1:
					_sit = 1;
					break;
				case 2:
					break;
				case 3:
					if (A->lastSit == 0) {
						_sit = 1;

					}
					break;

				}
			} else {

				switch (mode) {
				case 2:
					_sit = 1;
					break;
				case 4:
					if (A->lastSit == 1) {
						_sit = 1;
					}
					break;
				}
			}

		}

		if (A->i.size() > 0) {
			bool alert = false;
			bool input = false;
			bool alertError = false;
			for (std::map<int, int>::iterator it = A->i.begin(); it != A->i.end(); ++it) {

				input = true;
				if (A->inputI[it->first] == it->second) {
					alert = true;
				} else {
					alertError = true;
				}

			}

			if (input) {
				if (alert == false || A->inputModo == 2 && alertError) {
					_inp = 0;
				} else {
					_inp = 1;
				}
			}
		}

		if (_min == -1 && _max == -1 && _geo == -1 && _inp == -1 and _sit == -1) {
			//hr(2);
			return false;
		}

		if ((_min == -1 || _min == 1)
			and (_max == -1 || _max == 1)
			and (_geo == -1 || _geo == 1)
			and (_sit == -1 || _sit == 1)
			and (_inp == -1 || _inp == 1)) {

			short i_velocidad = 0;
			short i_geocerca = 0;
			short i_input = 0;
			short i_sitio = 0;

			InfoEvent E;

			E.codalarma = A->codalarma;
			E.codequipo = P->codequipo;
			E.fechaHora = P->fechaHora;
			E.trackId = P->trackId;
			E.activo = 1;
			E.status = 1;
			E.velocidad = i_velocidad;
			E.geocerca = i_geocerca;
			E.input = i_input;
			E.sitio = i_sitio;

			E.alarma = A->alarma;
			E.descripcion = A->descripcion;

			insertEvent(&E);


		}

		//cout << "test --> " << A->sitioModo << endl;
		return false;
	}

	bool Webcar::insideCircle(WCPoint* p, WCPoint* r, WCPoint* d) {
		float d1 = pow(p->lat - r->lat, 2) + pow(p->lng - r->lng, 2);
		float d2 = pow(d->lat - r->lat, 2) + pow(d->lng - r->lng, 2);

		return (d2 - d1) >= 0 ? true : false;
	}

	bool Webcar::insideCircle2(WCPoint* p, WCPoint* r, float d) {

		float dLat = (p->lat - r->lat) * PI / 180;

		float dLng = (p->lng - p->lng) * PI / 180;

		float a = sin(dLat / 2) * sin(dLat / 2) +
			cos(r->lat * PI / 180) * cos(p->lat * PI / 180) *
			sin(dLng / 2) * sin(dLng / 2);
		float c = 2 * atan2(sqrt(a), sqrt(1 - a));
		float d1 = R * c;

		return (d1 <= d) ? true : false;
	}

	bool Webcar::insidePolygon(std::vector<WCPoint>* list, WCPoint* p) {

		int counter = 0;
		float xInters = 0;
		int n = list->size();
		int i;

		WCPoint* p1, * p2;
		p1 = &list->at(0);


		for (i = 1; i <= n; i++) {
			p2 = &list->at(i % n);
			if (p->lng > min(p1->lng, p2->lng)) {
				if (p->lng <= max(p1->lng, p2->lng)) {
					if (p->lat <= max(p1->lat, p2->lat)) {
						if (p1->lng != p2->lng) {
							xInters = (p->lng - p1->lng) * (p2->lat - p1->lat) / (p2->lng - p1->lng) + p1->lat;
							if (p1->lat == p2->lat or p->lat <= xInters) {
								counter++;
							}
						}
					}
				}
			}
			p1 = p2;
		}

		return (counter % 2 == 0) ? false : true;
	}

	void Webcar::getPoint(std::string str, WCPoint* point) {
		std::vector<std::string> aux = explode(str, ' ');
		point->lat = stof(aux[0]);
		point->lng = stof(aux[1]);

	}

	void Webcar::insertEvent(InfoEvent* P) {

		try {

			//p_stmt = cn->prepareStatement(query.c_str());
			//codalarma, codequipo, hora, track_id, activo, status, velocidad, geocerca, input, sitio, alarma, descripcion
			stmtEvent->setInt(1, P->codalarma);
			stmtEvent->setInt(2, P->codequipo);
			stmtEvent->setDateTime(3, P->fechaHora.c_str());
			stmtEvent->setInt(4, P->trackId);

			stmtEvent->setInt(5, P->activo);
			stmtEvent->setInt(6, P->status);
			stmtEvent->setInt(7, P->velocidad);
			stmtEvent->setInt(8, P->geocerca);
			stmtEvent->setInt(9, P->input);
			stmtEvent->setInt(10, P->sitio);

			stmtEvent->setString(11, P->alarma.c_str());
			stmtEvent->setString(12, P->descripcion.c_str());



			if (stmtEvent->execute()) {
			}
			//delete res;


		} catch (sql::SQLException& e) {


			cout << endl << endl << "# ERR: SQLException in " << __FILE__;
			cout << endl << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << endl << "# ERR: " << e.what();
			cout << endl << " (MySQL error code: " << e.getErrorCode();
			//cout << ", SQLState: " << e.getSQLState().c_str() << " )" << endl;


		}
	}

	void Webcar::updateAlarm(InfoDBAlarm* P) {
		try {

			stmtAlarm->setInt(1, P->status);
			stmtAlarm->setInt(2, P->velocidad);

			stmtAlarm->setInt(3, P->geocerca);
			stmtAlarm->setInt(4, P->sitio);
			stmtAlarm->setInt(5, P->codequipo);
			stmtAlarm->setInt(6, P->codalarma);

			if (stmtAlarm->execute()) {
			}
			//delete res;


		} catch (sql::SQLException& e) {


			cout << endl << endl << "# ERR: SQLException in " << __FILE__;
			cout << endl << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << endl << "# ERR: " << e.what();
			cout << endl << " (MySQL error code: " << e.getErrorCode();
			//cout << ", SQLState: " << e.getSQLState().c_str() << " )" << endl;


		}

	}

	InfoDevice Webcar::getInfoDevice(std::string unitId) {
		
		sql::ResultSet* result = nullptr;

		InfoDevice R;

		try {
		

			stmtDevice->setString(1, unitId.c_str());
		
			if (stmtDevice->execute()) {

				result = stmtDevice->getResultSet();

				if (result->next()) {
					R.codequipo = result->getInt("codequipo");
					R.parametros = result->getString("parametros").c_str();
					R.params = explode(R.parametros, ',');

				}
				delete result;
			}

		

		} catch (sql::SQLException& e) {
			cout << endl << endl << "# ERR: SQLException in " << __FILE__;
			cout << endl << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << endl << "# ERR: " << e.what();
			cout << endl << " (MySQL error code: " << e.getErrorCode();
			//cout << ", SQLState: " << e.getSQLState().c_str() << " )" << endl;

		}
		return R;
	}

	void Webcar::insertTrack(std::string name, std::string track) {
		//track = "2012000581,20020319154431,-66.858390,10.247223,8,169,327.0,12,2,22419.6,1,14.69,0.01,8,0,19,0,,,,4.190";
		//track = "2012000581,,,,,,";
		if (stmtTrack == nullptr) {
			cPos["id_equipo"] = 2;
			cPos["fecha_hora"] = 3;
			cPos["longitud"] = 4;
			cPos["latitud"] = 5;
			cPos["velocidad"] = 6;
			cPos["heading"] = 7;
			cPos["altitud"] = 8;
			cPos["satelites"] = 9;
			cPos["event_id"] = 10;
			cPos["input"] = 11;
			cPos["millas"] = 12;
			cPos["analog_input_1"] = 13;
			cPos["analog_input_2"] = 14;
			cPos["analog_output"] = 15;
			cPos["output"] = 16;
			cPos["counter_1"] = 17;
			cPos["counter_2"] = 18;

			std::string qTracking = R"(INSERT INTO tracks_2020 
				(codequipo, id_equipo, fecha_hora, longitud, latitud,
				velocidad, heading, altitud, satelites,
				event_id, input, millas, 
				analog_input_1, analog_input_2, analog_output,
				output, counter_1, counter_2
				)
			VALUES (
				?,?,?,?,?,
				?,?,?,?,
				?,?,?,
				?,?,?,
				?,?,?

			)
			)";
			stmtTrack = cn->prepareStatement(qTracking);
		}
		
		InfoDevice info = getInfoDevice(name);
		int codequipo = info.codequipo;

		if (codequipo == 0) {
			return;
		}
		//std::vector<string> values = getItem(track.c_str());
		std::vector<string> values = split(track.c_str(), ',');
		std::vector<string> names = info.params;
		
		
		int nameSize = names.size();
		int valueSize = values.size();
		
		std::string ver = "";
		std::string field = "";
		std::string value = "";

		std::map<std::string, std::string> prm;
		for (int i = 0; i < nameSize; i++) {
			field = names.at(i).c_str();
			value = "0";

			if (i< values.size()) {
				value = values.at(i).c_str();
				
			}

			prm[field] = value;
			trackParams[field] = value;
		}
		/*
		for (std::map<std::string, std::string>::iterator it = prm.begin(); it != prm.end(); ++it) {

			printf("%8s", it->first.c_str());
			printf("%8s\n", it->second.c_str());
			

		}
		*/
		

		//std::cout << "Track " << track << "\nCODEQUIPO " << codequipo << "\n";
		int pos = 0;
		try {
			bool error = false;
			
			std::string sValue = "";
			stmtTrack->setInt(1, codequipo);
			
			for (auto itr = cPos.begin(); itr != cPos.end(); ++itr) {
				auto found = prm.find(itr->first.c_str());
				pos = itr->second;
				if (found != prm.end()) {
					sValue = found->second.c_str();
					//std::cout << " POS: " << pos <<" value:" << sValue << "\n";
					if (sValue == "0" && (pos == 3 || pos == 4 || pos == 5)) {
						error = true;
						//std::cout << " errrrrrrrrrrrrrr\n";
						continue;
					}
					stmtTrack->setString(itr->second, found->second.c_str());
				}else {
					
					stmtTrack->setString(itr->second, "0");
				}

				
			}
			
			
			
			if (error) {
				std::cout << "WEBCAR error: " << codequipo << "\n";
				return;
			}

			stmtTrack->execute();
			std::cout << "saving WEBCAR tracking from: " << codequipo << "\n";
			int trackId = 0;
			sql::ResultSet* result = nullptr;
			if (stmtLastId->execute()) {

				result = stmtLastId->getResultSet();

				if (result->next()) {
					trackId = result->getInt("last_id");
				}
				delete result;
			}
			
			WC::TrackParam P ({ trackId, codequipo, name.c_str(),trackParams["fecha_hora"], stof(trackParams["longitud"]), stof(trackParams["latitud"]),stoi(trackParams["velocidad"]), stoi(trackParams["input"]) });
			evalTrack(&P);


		} catch (sql::SQLException& e) {
			if (e.getErrorCode() == 1062) {
				cout << "WEBCAR ERR: DUPLICATE\n";
			}else {
				cout << endl << " (MySQL error code: " << e.getErrorCode();
			}
			//cout << "WEBCAR ERR: SQLException\n" ;
			
			//cout << endl << endl << "WEBCAR ERR: SQLException in " << __FILE__;
			//cout << "Error Position: " << pos << endl;
			//cout << "Track: " << track << endl;
			//cout << endl << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			//cout << endl << "WEBCAR ERR: " << e.what();
			
			
			//cout << ", SQLState: " << e.getSQLState().c_str() << " )" << endl;

		}
	}

	bool Webcar::saveTracking(int unitId, std::map<std::string, std::string> data) {
		

		try {
			std::vector<string> fields = {

				"unit_id", "device_id", "date_time", "longitude", "latitude",
				"speed", "heading", "altitude", "satellite", "event_id",
				"mileage", "input_status", "voltage_level_i1", "voltage_level_i2", "output_status",
				"voltage_output","pulse_i3", "pulse_i4", "info"
				
			};

			if (stmtTracking == nullptr) {

				stmtTracking = cn->prepareStatement(
					R"(INSERT IGNORE INTO tracks_2020 
						(
							codequipo, id_equipo, fecha_hora, longitud, latitud,
							velocidad, heading, altitud, satelites, event_id, 
							millas, input,  analog_input_1, analog_input_2, output,
							analog_output, counter_1, counter_2, info
						)
						VALUES (
							?,?,?,?,?,
							?,?,?,?,?,
							?,?,?,?,?,
							?,?,?,?
						)
					)");
			}

			InfoDevice info = getInfoDevice(data["device_id"].c_str());
			
			
			stmtTracking->setInt(1, info.codequipo);

			for (int i = 1; i < fields.size(); i++) {
				if (data[fields[i]] == "") {
					if (fields[i] == "date_time" || fields[i] == "longitude" || fields[i] == "longitude" ||
						fields[i] == "latitude" || fields[i] == "speed" || fields[i] == "heading" ||
						fields[i] == "altitude" || fields[i] == "satellite" || fields[i] == "event_id" ||
						fields[i] == "input_status" || fields[i] == "voltage_level_i1" || fields[i] == "voltage_level_i2" ||
						fields[i] == "pulse_i3" || fields[i] == "pulse_i4" || fields[i] == "battery_voltage" ||
						fields[i] == "voltage_output")
					{
						stmtTracking->setNull(i + 1, sql::DataType::INTEGER);
					}
					else {
						stmtTracking->setNull(i + 1, sql::DataType::VARCHAR);
					}
				}
				else {
					stmtTracking->setString(i + 1, data[fields[i]].c_str());
				}
				//std::cout << "\n i: " << i << " - " << fields[i] << " ";
			}

			stmtTracking->execute();

			
			std::cout << "saving WEBCAR tracking from: " << info.codequipo << "\n";
			int trackId = 0;
			sql::ResultSet* result = nullptr;
			if (stmtLastId->execute()) {

				result = stmtLastId->getResultSet();

				if (result->next()) {
					trackId = result->getInt("last_id");
				}
				delete result;
			}

			WC::TrackParam P({ trackId, info.codequipo, data["device_id"], data["date_time"], 
				stof(data["longitude"]), stof(data["latitude"]),
				stoi(data["speed"]), stoi(data["input_status"]) });
			evalTrack(&P);
		}

		catch (sql::SQLException& e) {
			if (e.getErrorCode() == 1062) {
				cout << "WEBCAR ERR: DUPLICATE\n";
			}
			else {
				cout << endl << " (MySQL error code: " << e.getErrorCode();
			}
		}

		return true;
	}


	AppConfig Webcar::loadConfig(const char* path) {

		//"C:\\source\\cpp\\XT\\XTServer\\config.json"
		FILE* fp = fopen(path, "rb"); // non-Windows use "r"

		if (fp == NULL) {
			perror("Error while opening the file.\n");
			exit(EXIT_FAILURE);
		}

		char* readBuffer;

		readBuffer = (char*)malloc(1500);
		rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
		//Document d;

		json.ParseStream(is);
		fclose(fp);

		//strcpy_s(info.appname, sizeof(info.appname), d["appname"].GetString());
		config.appname = json["appname"].GetString();
		config.port = json["port"].GetInt();
		config.max_clients = json["max_clients"].GetInt();
		config.version = json["version"].GetString();
		config.debug = json["debug"].GetBool();
		config.show_cache = json["show_cache"].GetBool();

		config.db.host = json["db"]["host"].GetString();
		config.db.port = json["db"]["port"].GetString();
		config.db.user = json["db"]["user"].GetString();
		config.db.pass = json["db"]["pass"].GetString();
		config.db.name = json["db"]["name"].GetString();

		/*
		strcpy_s(info.version, sizeof(info.version), d["version"].GetString());

		strcpy_s(info.db.dbname, sizeof(info.db.dbname), d["db"]["dbname"].GetString());
		strcpy_s(info.db.host, sizeof(info.db.host), d["db"]["host"].GetString());
		strcpy_s(info.db.user, sizeof(info.db.user), d["db"]["user"].GetString());
		strcpy_s(info.db.pass, sizeof(info.db.pass), d["db"]["pass"].GetString());
		strcpy_s(info.db.port, sizeof(info.db.port), d["db"]["port"].GetString());
		*/
		if (readBuffer != NULL) {
			readBuffer[0] = '\0';
			free(readBuffer);
		}

		return config;
	}

	std::string Webcar::encodeTracking(std::map<std::string, std::string> data)
	{
		std::vector<std::string> params = {
			"ID",
			"dateTime",
			"longitude",
			"latitude",
			"speed",
			"heading",
			"altitude",
			"satellites",
			"almCode",
			"mileage",
			"in-sta",
			"voltage_level_i1",
			"voltage_level_i2",
			"out-sta",
			"pulse_i3",
			"pulse_i4",
			"rtc"

		};

		
		std::string str = "";

		for (std::vector<std::string>::iterator it = params.begin(); it != params.end(); ++it) {
			if (str != "") {
				if (data[*it] != "") {
					str += "," + data[*it];
				}
				else {
					str += ",0";
				}

			}
			else {
				str = data[*it];
			}

		}

		return str;
	}

}