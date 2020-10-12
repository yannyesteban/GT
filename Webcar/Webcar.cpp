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

void Webcar::evalTrack(int id, int codequipo, float longitud, float latitud, int velocidad, int input) {


	stmtSpeed->setInt(1, velocidad);
	if (!stmtSpeed->execute()) {
		cout << "todo mal" << endl;
	}

	//sql::PreparedStatement* p_stmt;
	sql::ResultSet* result = nullptr;
		//bool isRead = false;


	try {
	

		//stmtMain->setInt(1, velocidad);
		stmtMain->setInt(1, codequipo);
	
		cout << "stage zero " << codequipo << endl;

		std::map<int, InfoAlarm> info;
		
		std::string inputI;
		std::string inputOn;

		std::string inputOff;
		if (stmtMain->execute()) {
			string _input = toBinR(input);
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
			evalAlarm(it->second);
		}
		//delete p_stmt;

	} catch (sql::SQLException& e) {
		cout << endl << endl << "# ERR: SQLException in " << __FILE__;
		cout << endl << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << endl << "# ERR: " << e.what();
		cout << endl << " (MySQL error code: " << e.getErrorCode();
	}


}

bool Webcar::evalAlarm(InfoAlarm alarm) {

	short _min = -1;



	cout << "test --> " << alarm.sitioModo << endl;
	return false;
}

