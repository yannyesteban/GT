#pragma once
#include <iostream>
#include <sstream>
#include <regex>
#include <boost/algorithm/string.hpp>
#include <string>
#include <vector>
#include <map>
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filereadstream.h"

int checkSum(char* buf, long bufLen);
class Cmd
{
public:
	Cmd();
	std::string encode(std::string ss, std::map<std::string, std::string> data);
	std::string encodeToArray(std::vector<std::string> params);
	std::string toList(std::vector<std::string>, std::string sep);
private:

};

