#include "Webcar.h"
#include<string>
using namespace std;

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

std::vector<std::string> explode(std::string const& s, char delim) {
	std::vector<std::string> result;
	std::istringstream iss(s);

	for (std::string token; std::getline(iss, token, delim); )
	{
		result.push_back(std::move(token));
	}

	return result;
}

Webcar::Webcar(InfoDB pInfo) :
	info(pInfo), driver(nullptr),
	cn(nullptr),
	stmt(nullptr),
	result(nullptr) {
}

Webcar::~Webcar() {
	delete stmtMain;
}

bool Webcar::connect() {
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

		stmtMain = cn->prepareStatement(qMain);
		stmtSpeed = cn->prepareStatement(qSpeedVar);
		stmtEvent = cn->prepareStatement(qInsertEvent);

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
void Webcar::evalTrack(InfoParam* P){
	/*
	InfoParam P{
		id, codequipo, longitud, latitud, velocidad, input
	};
	*/
	stmtSpeed->setInt(1, P->velocidad);
	if (!stmtSpeed->execute()) {
		cout << "todo mal" << endl;
	}

	//sql::PreparedStatement* p_stmt;
	sql::ResultSet* result = nullptr;
		//bool isRead = false;


	try {
	

		//stmtMain->setInt(1, velocidad);
		stmtMain->setInt(1, P->codequipo);
	
		cout << "stage zero " << P->codequipo << endl;

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

				

				info[codalarma].velMin = result->getInt("vel_min");
				info[codalarma].velMax= result->getInt("vel_max");

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
				cout << "vel max " << info[codalarma].velMax << endl;

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

bool Webcar::evalAlarm(InfoAlarm * A, InfoParam * P) {

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

		string query = R"(
			INSERT INTO alarma_eventos
			(codalarma, codequipo, hora, track_id, activo, status, velocidad, geocerca, input, sitio, alarma, descripcion)
			VALUES
			($kk, $codequipo, '$fecha_hora', $id, 1, 1, $i_velocidad, $i_geocerca, $i_input, $i_sitio, '$_alarma', '$_descripcion'))";

			
	}

	cout << "test --> " << A->sitioModo << endl;
	return false;
}

bool Webcar::insideCircle(WCPoint* p, WCPoint* r, WCPoint* d) {
	float d1 = pow(p->lat - r->lat, 2) + pow(p->lng - r->lng, 2);
	float d2 = pow(d->lat - r->lat, 2) + pow(d->lng - r->lng, 2);

	return (d2 - d1) >= 0? true: false;
}

bool Webcar::insideCircle2(WCPoint* p, WCPoint* r, float d) {

	float dLat = (p->lat - r->lat) * PI / 180;

	float dLng = (p->lng - p->lng) * PI / 180;

	float a = sin(dLat / 2) * sin(dLat / 2) +
		cos(r->lat * PI / 180) * cos(p->lat * PI / 180) *
		sin(dLng / 2) * sin(dLng / 2);
	float c = 2 * atan2(sqrt(a), sqrt(1 - a));
	float d1 = R * c;
	
	return (d1 <= d)? true: false;
}

bool Webcar::insidePolygon(std::vector<WCPoint>* list, WCPoint* p) {

	int counter = 0;
	float xInters = 0;
	int n = list->size();
	int i;

	WCPoint *p1, *p2;
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

	return (counter % 2 == 0)? false: true;
}

void Webcar::getPoint(std::string str, WCPoint * point) {
	std::vector<std::string> aux = explode(str, ' ');
	point->lat = stof(aux[0]);
	point->lng = stof(aux[1]);

}

void Webcar::insertEvent(InfoParam * P) {

	try {

		//p_stmt = cn->prepareStatement(query.c_str());
		//codalarma, codequipo, hora, track_id, activo, status, velocidad, geocerca, input, sitio, alarma, descripcion
		stmtEvent->setInt(1, P->codalarma);
		stmtEvent->setInt(2, P->codequipo);
		stmtEvent->setDateTime(3, P->fechaHora);
		stmtEvent->setInt(4, P->tr);


		p_stmt->setInt(2, request->commandId);
		p_stmt->setString(3, request->message);
		p_stmt->setString(4, to_string(request->index).c_str());
		p_stmt->setInt(5, request->index);
		p_stmt->setString(6, request->user);
		p_stmt->setInt(7, request->type);
		p_stmt->setInt(8, request->mode);

		if (p_stmt->execute()) {
		}
		//delete res;
		delete p_stmt;

	} catch (sql::SQLException& e) {


		cout << endl << endl << "# ERR: SQLException in " << __FILE__;
		cout << endl << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << endl << "# ERR: " << e.what();
		cout << endl << " (MySQL error code: " << e.getErrorCode();
		//cout << ", SQLState: " << e.getSQLState().c_str() << " )" << endl;


	}
}

