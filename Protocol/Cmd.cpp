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
