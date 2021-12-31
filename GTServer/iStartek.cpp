
#include "Tool.h"
#include "iStartek.h"

using namespace rapidjson;

bool GT::iStartek::isSynch(ConnInfo Info)
{
	return false;
}

bool  GT::iStartek::isMe(ConnInfo Info) {
	iStartekHeader* sync_msg = (iStartekHeader*)Info.buffer;

	std::string header = "";

	header += sync_msg->token[0];
	header += sync_msg->token[1];
	if (header == "&&") {
		return true;
	}
	return false;
}

std::string GT::iStartek::getTracking(ConnInfo Info)
{
	return std::string();
}

std::string GT::iStartek::getTracking(const std::string s)
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
	
	std::map<std::string, std::string> map = getEventData(s);
	std::string str = "";

	for (std::vector<std::string>::iterator it = params.begin(); it != params.end(); ++it) {
		if (str != "") {
			if (map[*it] != "") {
				str += "," + map[*it];
			}
			else {
				str += ",0";
			}
			
		}
		else {
			str = map[*it];
		}

	}

	return str;
}

std::list<std::string> GT::iStartek::getTrackingList(const std::string s)
{
	
	//std::cout << "getTrackingList\n\n";
	std::list<std::string> list1 = Tool::split3(s.c_str(), ',');
	return list1;
}

std::map<std::string, std::string> GT::iStartek::getEventData(const std::string s)
{
	
	std::vector<std::string> params = {
		"packHead",
		"ID",
		"cmd",
		"almCode",
		"almData",
		"dateTime",
		"fixFlag",
		"latitude",
		"longitude",
		"satellites",
		"HDOP",
		"speed",
		"heading",
		"altitude",
		"odometer",
		"BSI",
		"CSQQuanlity",
		"system-sta",
		"in-sta",
		"out-sta",
		"extV",
		"proCode",
		"fuleLiter",
		"tempSensor",
		//"OBD-data",
		"checksum",
		"r",
		"n"
		

	};

	std::list<std::string> li = Tool::split3(s.c_str(), ',');
	std::map<std::string, std::string> map;
	int i = 0;
	unsigned int value;
	std::string data = "";
	
	for (std::list<string>::iterator it = li.begin(); it != li.end(); it++) {

		if (i >= params.size()) {
			i++;
			continue;
		}
		if (params.at(i) == "in-sta" || params.at(i) == "out-sta") {
			
			std::stringstream ss;
			ss << std::hex << it->c_str();
			ss >> value;
			map[params.at(i).c_str()] = std::to_string(value);
		}
		else {
			map[params.at(i).c_str()] = it->c_str();
		}
		
		printf("%12s", params.at(i).c_str());
		printf("%16s\n", it->c_str());
		
		i++;
	}
	
	

	//Document main;
	//main.SetObject();

	Document infoDoc;
	infoDoc.SetObject();

	Value info;// (kObjectType);
	Value msg;

	auto bsi = Tool::splitv(map["BSI"].c_str(), '|');
	if (bsi.size() == 4) {

		

		Document json;
		json.SetObject(); // Make doc an object !
		

		
		//char buffer[512];
		//int len = sprintf(buffer, "%s", response->message);
		msg.SetString(bsi[0].c_str(), strlen(bsi[0].c_str()), infoDoc.GetAllocator());
		json.AddMember("MCC", msg, infoDoc.GetAllocator());


		msg.SetString(bsi[1].c_str(), strlen(bsi[1].c_str()), infoDoc.GetAllocator());
		json.AddMember("MNC", msg, infoDoc.GetAllocator());

		msg.SetString(bsi[2].c_str(), strlen(bsi[2].c_str()), infoDoc.GetAllocator());
		json.AddMember("LAC", msg, infoDoc.GetAllocator());

		msg.SetString(bsi[3].c_str(), strlen(bsi[3].c_str()), infoDoc.GetAllocator());
		json.AddMember("CI", msg, infoDoc.GetAllocator());

		//info.AddMember("v", json, infoDoc.GetAllocator());

		infoDoc.AddMember("BSI", json, infoDoc.GetAllocator());

	}



	auto extV = Tool::splitv(map["extV"].c_str(), '|');

	if (extV.size() > 0) {
		Document json;
		json.SetArray(); // Make doc an object !
		for (int i = 0; i < extV.size(); i++) {
			msg.SetString(extV[i].c_str(), strlen(extV[i].c_str()), infoDoc.GetAllocator());
			json.PushBack(msg, infoDoc.GetAllocator());
		}


		infoDoc.AddMember("extV", json, infoDoc.GetAllocator());

	}


	auto fuleLiter = Tool::splitv(map["fuleLiter"].c_str(), '|');

	if (fuleLiter.size() > 0) {
		Document json;
		json.SetArray(); // Make doc an object !
		for (int i = 0; i < fuleLiter.size(); i++) {
			msg.SetString(fuleLiter[i].c_str(), strlen(fuleLiter[i].c_str()), infoDoc.GetAllocator());
			json.PushBack(msg, infoDoc.GetAllocator());
		}
		infoDoc.AddMember("fuleLiter", json, infoDoc.GetAllocator());
	}


	auto tempSensor = Tool::splitv(map["tempSensor"].c_str(), '|');

	if (tempSensor.size() > 0) {
		Document json;
		json.SetArray(); // Make doc an object !
		for (int i = 0; i < tempSensor.size(); i++) {
			msg.SetString(tempSensor[i].c_str(), strlen(tempSensor[i].c_str()), infoDoc.GetAllocator());
			json.PushBack(msg, infoDoc.GetAllocator());
		}
		infoDoc.AddMember("tempSensor", json, infoDoc.GetAllocator());
	}

	msg.SetString(map["fixFlag"].c_str(), strlen(map["fixFlag"].c_str()), infoDoc.GetAllocator());
	infoDoc.AddMember("fixFlag", msg, infoDoc.GetAllocator());


	msg.SetString(map["HDOP"].c_str(), strlen(map["HDOP"].c_str()), infoDoc.GetAllocator());
	infoDoc.AddMember("HDOP", msg, infoDoc.GetAllocator());

	

	
	msg.SetString(map["CSQQuanlity"].c_str(), strlen(map["CSQQuanlity"].c_str()), infoDoc.GetAllocator());
	infoDoc.AddMember("CSQQuanlity", msg, infoDoc.GetAllocator());

	msg.SetString(map["proCode"].c_str(), strlen(map["proCode"].c_str()), infoDoc.GetAllocator());
	infoDoc.AddMember("proCode", msg, infoDoc.GetAllocator());


	StringBuffer sb;
	Writer<StringBuffer> wr(sb);

	infoDoc.Parse(sb.GetString());

	infoDoc.Accept(wr);

	std::cout << "JSON " << sb.GetString() << "\n\n";

	std::cout << "BSI " << map["BSI"].c_str() << "\n\n";
	
	std::cout << "bsi size " << bsi.size() << "\n\n";
	std::cout << "XXX " << bsi[0] << "\n\n";

	return map;
}

std::map<std::string, std::string> GT::iStartek::getResponse(const std::string s)
{
	std::vector<std::string> params = {
		"packHead",
		"ID",
		"cmd",
		"almCode",
		"almData",
		"dateTime",
		"fixFlag",
		"latitude",
		"longitude",
		"satellites",
		"HDOP",
		"speed",
		"heading",
		"altitude",
		"odometer",
		"BSI",
		"CSQ-quanlity",
		"system-sta",
		"in-sta",
		"out-sta",
		"ext-V",
		"pro-code",
		"fule_liter",
		"temp-sensor",
		//"OBD-data",
		"checksum",
		"r",
		"n"


	};

	std::list<std::string> li = Tool::split3(s.c_str(), ',');
	std::map<std::string, std::string> map;
	int i = 0;
	std::string data = "";

	for (std::list<string>::iterator it = li.begin(); it != li.end(); it++) {
		if (i >= params.size()) {
			i++;
			continue;
		}
		if (i <= 2) {
			map[params.at(i).c_str()] = it->c_str();
		}
		else {
			data += ((data != "")?",":"") + std::string(it->c_str());
		}
		
		//printf("%12s", params.at(i).c_str());
		//printf("%16s\n", it->c_str());

		i++;
	}

	

	map["data"] = data;
	return map;
}
