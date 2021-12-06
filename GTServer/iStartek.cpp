#include "iStartek.h"
#include "Tool.h"

bool GT::iStartek::isSynch(ConnInfo Info)
{
	return false;
}

std::string GT::iStartek::getTracking(ConnInfo Info)
{
	return std::string();
}

std::list<std::string> GT::iStartek::getTrackingList(const std::string s)
{
	
	std::cout << "getTrackingList\n\n";
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
	for (std::list<string>::iterator it = li.begin(); it != li.end(); it++) {
		map[params.at(i).c_str()] = it->c_str();
		printf("%12s", params.at(i).c_str());
		printf("%16s\n", it->c_str());
		i++;
	}
	return map;
}
