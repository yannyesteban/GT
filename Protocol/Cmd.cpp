#include "Cmd.h"


int checkSum(char* buf, long bufLen) {
	static char tmpBuf[4];
	long idx;
	unsigned int cks;

	for (idx = 0L, cks = 0; idx < bufLen; cks += (unsigned int)buf[idx++]);
	//sprintf(tmpBuf, "%03d", (unsigned int)(cks % 256));
	return(cks % 256);
}

Cmd::Cmd()
{
}

std::string Cmd::encode(std::string ss, std::map<std::string, std::string> data)
{
	auto str = ss;
	std::smatch m;

	//std::regex Pala("[0-9.a-zA-ZñÑáéíóúÁÉÍÓÚäëïöüÄËÏÖÜ]+");
	std::regex Pala("<(\\w+)>");

	int i = 1;
	char tmpBuf[3];
	
	std::string rep = "";
	std::string temp = "";
	
	while (std::regex_search(ss, m, Pala)) {

		if (m[i].str() == "packlen") {
			temp = encode(data["fpacklen"], data);
			
			rep = std::to_string(temp.length());
		}
		else if (m[i].str() == "checksum") {
			temp = encode(data["fchecksum"], data);
			int check = (unsigned int)checkSum((char*)temp.c_str(), temp.size());
			sprintf_s(tmpBuf, "%02X", check);
			rep = tmpBuf;
		}
		else {
			rep = data[m[i].str()];
		}
		
		str = regex_replace(str, std::regex(m[0].str()), rep);

		ss = m.suffix().str();
	}

	return str;
}

std::map<std::string, std::string> Cmd::encode(std::string exp, std::vector<std::string> names, std::string message) {

	std::regex regexp(exp);
	
	std::smatch m;


	std::map<std::string, std::string> map;

	std::regex_search(message, m, regexp);
	
	for (int i = 0; i < m.size(); i++) {
		

		if (i < names.size()) {
			map[names[i]] = m[i+1].str();
			//std::cout << "\nm [" << names[i] << "]: " << m[i+1].str();
		}
	}
	return map;
	/*
	printf("\n\n");

	for (std::map<std::string, std::string>::iterator it = map.begin(); it != map.end(); ++it) {
		printf("%20s", it->first.c_str());
		printf("%50s\n", it->second.c_str());

	}

	return map;*/
}

std::string Cmd::encodeToArray(std::vector<std::string> params)
{

	rapidjson::Document json;
	json.SetArray();

	rapidjson::Document::AllocatorType& allocator = json.GetAllocator();

	std::vector<std::string> result;
	for (std::vector<std::string>::iterator it = params.begin(); it != params.end(); it++) {

		rapidjson::Value key(it->c_str(), it->size(), allocator);
		json.PushBack(key, allocator);

	}

	rapidjson::StringBuffer sbuffer;
	rapidjson::Writer<rapidjson::StringBuffer> wr(sbuffer);

	json.Parse(sbuffer.GetString());

	json.Accept(wr);

	//std::cout << " JSON-STRING-ARRAY: " << sbuffer.GetString() << "\n";
	return sbuffer.GetString();
}

std::string Cmd::toList(std::vector<std::string> data, std::string sep)
{
	std::string list = "";
	
	for (int i = 0; i < data.size(); i++) {

		if (list != "") {
			list += sep + data[i];
		}
		else {
			list += data[i];
		}
	}

	return list;
}

std::string Cmd::getName(std::string exp, std::string str) {

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


std::vector<std::string> Cmd::splitParams(const std::string& s, char delim) {
	std::vector<std::string> result;
	std::stringstream ss(s);
	std::string item;

	while (getline(ss, item, delim)) {
		result.push_back(item);
	}

	return result;
}

int Cmd::toInteger(std::string value)
{
	
	int i;
	std::istringstream(value) >> i;

	
	return i;
}
