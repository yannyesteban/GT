// Proto.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <iomanip> // para la fecha

#include "Types.h"
#include "ProtoDB.h"

#include <regex>

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filereadstream.h"
#include "Cmd.h"

using namespace GT;
int getCheckSum(char* buf, long bufLen) {
	static char tmpBuf[4];
	long idx;
	unsigned int cks;

	for (idx = 0L, cks = 0; idx < bufLen; cks += (unsigned int)buf[idx++]);
	//sprintf(tmpBuf, "%03d", (unsigned int)(cks % 256));
	return(cks % 256);
}

std::string getName(std::string exp, std::string str) {

	std::smatch m;

	std::string name = "";

	std::regex Exp(exp.c_str());

	while (std::regex_search(str, m, Exp)) {

		if (m[1] != "") {
			name = m[1].str();
			break;
		}

		str = m.suffix().str();
	}

	return name;

}
std::string process(std::string ss, std::map<std::string, std::string> data) {

	std::smatch m;


	auto xx = ss;

	//std::regex Pala("[0-9.a-zA-ZñÑáéíóúÁÉÍÓÚäëïöüÄËÏÖÜ]+");
	std::regex Pala("<(\\w+)>");

	int i = 1;
	char tmpBuf[3];
	std::string rep = "";
	std::string temp = "";
	//std::cout << "\nBUFFER " << buffer << "\n-----------------\n";
	while (std::regex_search(ss, m, Pala)) {

		if (m[i].str() == "packlen") {
			temp = process(data["fpacklen"], data);
			std::cout << " temp: " << temp.length() << "\n\n";
			rep = std::to_string(temp.length());
		}
		else if (m[i].str() == "checksum") {
			temp = process(data["fchecksum"], data);
			std::cout << " temp: " << temp.length() << "\n\n";

			int check = (unsigned int)getCheckSum((char*)temp.c_str(), temp.size());

			sprintf(tmpBuf, "%02X", check);

			rep = tmpBuf;
		}
		else {
			rep = data[m[i].str()];
		}

		//for (int i = 0; i < m.size(); i++) {

				//w[len++] = m[i].str();
		std::cout << "\n" << i << " : " << m[i].str() << "....\n";
		xx = regex_replace(xx, std::regex(m[0].str()), rep);

		//}

		ss = m.suffix().str();

	}

	return xx;
}


std::map<std::string, std::string> decodeExp(std::string s, std::string exp, std::vector<std::string> names) {
	
	regex regexp(exp);
	smatch m;


	std::map<std::string, std::string> map;

	regex_search(s, m, regexp);

	for (int i = 1; i < m.size(); i++) {
		if (i < names.size()) {
			map[names[i - 1]] = m[i].str();
			cout << "\nm [" << names[i - 1] << "]: " << m[i].str();
		}
	}
	printf("\n\n");

	for (std::map<std::string, std::string>::iterator it = map.begin(); it != map.end(); ++it) {
		printf("%20s", it->first.c_str());
		printf("%50s\n", it->second.c_str());

	}

	return map;
}
int test() {


	std::vector<string> names = {
		"packno", "packlen", "device_id", "cmd", "event_id", "alarm", "date_time", "fix_flag","latitude", "longitude", 
		"satellite", "hdop", "speed", "heading", "altitude", "odometer", "bs_inf", "csq", "system_sta", 
		"input_status", "output_status", "voltage", "pro_code","fule_liter", "temp_sensor", "obd", "checksum"
	};
	string mystr = "&&y178,2024000036,000,0,,220413183959,A,10.502770,-66.848846,13,1.0,0,288,939,211,734|2|02CE|002A2101,24,0000003F,06,00,057E|019D|0000|0000,1,010000|020000,sen,782|30||33|80|12|115|10|26LA3";
	std::string exp = "^&&(\\w)(\\d+),([^,]+),([^,]+),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*),([^,]*)(?:,([^,]*))?([\\w]{2})";

	
	return 4;

	// regex expression for pattern to be searched 
	//regex regexp("^&&(\\w)(\\d+),([^,]+)+([\\w]{2})$");
	regex regexp(exp);
	//regex regexp("(?:&&)");
	// flag type for determining the matching behavior (in this case on string objects)
	smatch m;

	// regex_search that searches pattern regexp in the string mystr  
	regex_search(mystr, m, regexp);

	cout << "String that matches the pattern: ["<< m[1].str() << "]\n";
	
	for (int i = 1; i < m.size(); i++) {

		cout << "\nm ["<<  names[i-1] << "]: " << m[i].str();

	}
	//return 0;
	for (const auto& x : m) {
		cout << x << "\n ";
	}
		


	return 0;
}

int encodeJson(std::map<std::string, std::string> data) {
	rapidjson::Document json;
	json.SetObject();


	std::string message;
	rapidjson::Value msg;

	for (std::map<std::string, std::string>::iterator it = data.begin(); it != data.end(); ++it) {
		printf("%20s", it->first.c_str());
		printf("%50s\n", it->second.c_str());
		message = it->second;
		rapidjson::Value key(it->first.c_str(), it->first.size(), json.GetAllocator());
		msg.SetString(message.c_str(), message.size(), json.GetAllocator());
		json.AddMember(key, msg, json.GetAllocator());
	
	}



	rapidjson::StringBuffer sbuffer;
	rapidjson::Writer<rapidjson::StringBuffer> wr(sbuffer);

	json.Parse(sbuffer.GetString());

	json.Accept(wr);

	std::cout << " JSON : " << sbuffer.GetString() << "\n\n";
}

int main()
{
	std::string mystr2 = "";

	setlocale(LC_CTYPE, "Spanish");
	system("cls");

	std::string ss = "&&y178,2024000036,000,0,,220413183959";
	std::string xx = getName("&&[^,]+,(\\d+),\\d+", ss);

	//1012000372,2022-05-17 20:24:50
	std::cout << xx;

	return 1;

	//return test2();
	//return test();
	InfoDB infoDB = {

			"localhost",
			"3306",
			"gt",
			"root",
			"123456",
			false

	};
	auto db = new ProtoDB(infoDB);
	db->connect();

	EncodeCommand encodeInfo = db->infoCommand(4032, "109", 5);


	std::map<std::string, std::string> cmdData;
	cmdData["id"] = encodeInfo.deviceId;
	cmdData["pass"] = encodeInfo.pass;
	cmdData["cmd"] = encodeInfo.command;


	cmdData["fpacklen"] = encodeInfo.packlen;
	cmdData["fchecksum"] = encodeInfo.checksum;
	cmdData["index"] = std::to_string(encodeInfo.index);
	cmdData["oparams"] = encodeInfo.listOParams;
	cmdData["params"] = encodeInfo.listParams;
	cmdData["packno"] = encodeInfo.packno;
	

	Cmd cmd;
	std::string x = cmd.encode(encodeInfo.commandExp, cmdData);

	std::cout << "new command es : " << x << "\n\n";

	return 0;


	db->loadProtocols(); 


	mystr2 = "&&532,5024000001,252,yan,est,nue,1247E4";
	mystr2 = "&&G15,2024000025,800D4";

	db->decodeCommand(mystr2, db->mProto[43]);

	auto list1 = db->decodeParams("", db->mProto[43]);

	db->encodeToArray(list1);

	return 0;
	
	std::cout << db->mProto[43].tracking;

	string mystr = "&&y178,2024000036,000,0,,220413183959,A,10.502770,-66.848846,13,1.0,0,288,939,211,734|2|02CE|002A2101,24,0000003F,06,00,057E|019D|0000|0000,1,010000|020000,sen,782|30||33|80|12|115|10|26LA3";
	mystr = "&&&y178,2024000036,000,0,,220413183969,A,10.502770,-66.848846,3,1.0,0,288,939,211,734|2|02CE|002A2101,24,0000003F,06,00,057E|019D|0000|0000,1,010000|020000,sen,782|30||33|80|12|115|10|26LA3";
	auto data = db->decodeTracking(mystr, db->mProto[43]);
	db->evalData(data, db->mProto[43]);
	db->saveTrack(4032, data);

	if (db->isTrack(mystr, db->mProto[43])) {
		std::cout << "\n\n Si ES \n\n";
	}
	else {
		std::cout << "\n\n NOOOOOOOO \n\n";
	}
	//std::cout << db->commandEncode(4032, 2597, 5, 1, "1,3,1,") << "\n";
	
	db->decodeParams("yanny,esteban", db->mProto[43]);

	
	mystr = "&&532,5024000001,252,yan,est,nue,1247E4";

	auto data2 = db->decodeCommand(mystr, db->mProto[43]);
	return 0;

	std::cout << db->commandEncode(4032, 2597, 5, 1, "1,3,1,") << "\n";
	std::cout << db->commandEncode(4032, 2597, 5, 1, "1,3,1,") << "\n";
	std::cout << db->commandEncode(4032, 2549, 1, 1, "1,5,1,") << "\n";
	std::cout << db->commandEncode(4032, 2549, 1, 1, "1,5,1,") << "\n";
	return 0;

	db->saveCmd({
		4032,
		"2597",
		5,
		2,
		1,
		"mi comando 222",
		"5,55,555,5",
		"123",
		"",
		"panda"

		});

	db->saveCmd({
		4032,
		"2597",
		5,
		1,
		1,
		"mi comando 222",
		R"(["yan","est","nue","1247"])",
		"123",
		"",
		"panda"

		});

	db->commandEncode(4032, 2597, 5, 1, "1,3,1,");
	db->commandEncode(4032, 2597, 4, 1, "1,3,1,");
	db->commandEncode(4032, 2549, 1, 1, "1,5,1,");
	//db->commandEncode(4032, 2549, 1, 1, "1,8,1,");

	InfoCmd info2 = {
		4032,
		"2597",
		1,
		1,
		1,
		"mi comando x252",
		"6,78,65",
		"123",
		"",
		"panda"

	};
	db->saveCmd(info2);
	return 1;
	int unitId;
	int commandId;
	int index;
	int mode;
	int status;
	std::string name;
	std::string params;
	std::string query;
	std::string values;
	std::string user;
	InfoCmd info = {
		4032,
		"202",
		1,
		2,
		1,
		"mi comando 222",
		"[4,5,6, 8, 9]",
		"123",
		"",
		"panda"

	};
	db->saveCmd(info);
	db->saveCmd(info);
	db->saveCmd(info);

	return 1;

	


}

// Ejecutar programa: Ctrl + F5 o menú Depurar > Iniciar sin depurar
// Depurar programa: F5 o menú Depurar > Iniciar depuración

// Sugerencias para primeros pasos: 1. Use la ventana del Explorador de soluciones para agregar y administrar archivos
//   2. Use la ventana de Team Explorer para conectar con el control de código fuente
//   3. Use la ventana de salida para ver la salida de compilación y otros mensajes
//   4. Use la ventana Lista de errores para ver los errores
//   5. Vaya a Proyecto > Agregar nuevo elemento para crear nuevos archivos de código, o a Proyecto > Agregar elemento existente para agregar archivos de código existentes al proyecto
//   6. En el futuro, para volver a abrir este proyecto, vaya a Archivo > Abrir > Proyecto y seleccione el archivo .sln
