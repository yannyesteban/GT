#pragma once

#define BOOST_RESULT_OF_USE_DECLTYPE
#include <iostream>
#include <sstream>
#include <algorithm>
#include <list>
#include <iterator>
#include <regex>
#include <boost/algorithm/string.hpp>
#include <string>
#include <vector>



using namespace std;
namespace GT {
	

	class Tool {

	public:
		static std::string alphaNumeric(std::string ss);
		static list<string> split(string text, char delimeter);
		static void test();
		static void getItem(std::string w[], int wLength, int& len, const char* buffer);
		static void getItem(std::list<std::string> * w, int& len, const char* buffer);
		static void getItem(std::vector<std::string>* w, int& len, const char* buffer);
		static void getCommand(std::string w[], int& len, const char* buffer);
		static void getSendCommand(std::string w[], int& len, const char* buffer);
		static void getTracking(std::string w[], int& len, const char* buffer);
		template <class Container>
		static void split2(const std::string& str, Container& cont, char delim);
		static list<string> split3(const string& s, char delim);
		static vector<string> splitv(const string& s, char delim);
		static int getCheckSum(char* buf, long bufLen);
	};
}


