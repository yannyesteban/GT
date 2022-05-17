#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filereadstream.h"
#include <cstdio>
#include <iostream>
#include "Server.h"

namespace GT {

	

	class Config {

	public:

		static rapidjson::Document d;
		static AppConfig load(const char* path);

		static AppConfig getInfo() {
			return info;
		}
	private:
		static AppConfig info;

	};

}


