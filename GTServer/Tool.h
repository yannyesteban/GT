#pragma once

#define BOOST_RESULT_OF_USE_DECLTYPE
#include <iostream>
#include <list>
#include <iterator>
#include <regex>
#include <boost/algorithm/string.hpp>

using namespace std;
namespace GT {
	class Tool {

	public:
		static list<string> split(string text, char delimeter);
		static void test();
		static void getItem(std::string w[], int& len, const char* buffer);
		static void getCommand(std::string w[], int& len, const char* buffer);
		
	};
}


