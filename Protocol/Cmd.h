#pragma once
#include <iostream>
#include <sstream>
#include <regex>
#include <boost/algorithm/string.hpp>
#include <string>
#include <vector>
#include <map>
int checkSum(char* buf, long bufLen);
class Cmd
{
public:
	Cmd();
	std::string encode(std::string ss, std::map<std::string, std::string> data);
private:

};

