#include "Tool.h"




using namespace std;



namespace GT {

	int Tool::getCheckSum(char* buf, long bufLen) {
		static char tmpBuf[4];
		long idx;
		unsigned int cks;

		for (idx = 0L, cks = 0; idx < bufLen; cks += (unsigned int)buf[idx++]);
		//sprintf(tmpBuf, "%03d", (unsigned int)(cks % 256));
		return(cks % 256);
	}

	std::string Tool::alphaNumeric(std::string ss) {

		std::smatch m;
		//std::string ss("\xFA\xF8\xE7\x03\xB1yanny nunez,4.5,-8.369898,esteban");

		std::regex Pala("([a-zA-Z0-9\\-\\,\\.\\s\\::]+)");

		while (std::regex_search(ss, m, Pala)) {
			for (int i = 0; i < m.size(); i++) {
				std::cout << " *** " << m[i].str() << "\n";
				return m[i].str();
			}

			ss = m.suffix().str();
		}
		return "";
	}

	list<string> Tool::split3(const string& s, char delim) {
		list<string> result;
		stringstream ss(s);
		string item;

		while (getline(ss, item, delim)) {
			result.push_back(item);
		}

		return result;
	}

	vector<string> Tool::splitv(const string& s, char delim) {
		vector<string> result;
		stringstream ss(s);
		string item;

		while (getline(ss, item, delim)) {
			result.push_back(item);
		}

		return result;
	}

	list<string> Tool::split(string text, char delimeter) {

		list<string>results;

		boost::split(results, text, [delimeter](char c) { return c == delimeter; });

		return results;

	}

	void Tool::test() {
		string text = "one,two,three,four,five";
		char delimeter = ',';
		list<string>results;

		boost::split(results, text, [delimeter](char c) { return c == delimeter; });
		//boost::split(results, text, boost::is_any_of((char)token));
		list <string> ::iterator it;
		cout << "text: " << text << "Delimeter: " << delimeter << std::endl;
		for (it = results.begin(); it != results.end(); ++it) {
			cout << "=>" << *it << endl;
		}

		cout << "*****\n";


	}
	void Tool::getItem(std::string w[], int wLength, int& len, const char* buffer) {
		
		std::smatch m;
		std::string ss(buffer);

		//std::regex Pala("[0-9.a-zA-ZñÑáéíóúÁÉÍÓÚäëïöüÄËÏÖÜ]+");
		std::regex Pala("[^,]+");
		len = 0;
		//std::cout << "\nBUFFER " << buffer << "\n-----------------\n";
		while (std::regex_search(ss, m, Pala)) {
			
			for (int i = 0; i < m.size(); i++) {
				if (len < wLength) {
					w[len++] = m[i].str();
					//std::cout << "\nm-Size " << m.size() << "\n I: " << i <<  ", LENGTH " << len << "\n .........\n";
				}
				
			}
			
			ss = m.suffix().str();

		}

	}

	void Tool::getItem(std::vector<std::string>* list, int& len, const char* buffer) {
		std::smatch m;
		std::string ss(buffer);

		//std::regex Pala("[0-9.a-zA-ZñÑáéíóúÁÉÍÓÚäëïöüÄËÏÖÜ]+");
		std::regex Pala("[^,]+");

		while (std::regex_search(ss, m, Pala)) {

			for (int i = 0; i < m.size(); i++) {

				list->push_back(m[i].str());

			}

			ss = m.suffix().str();
		}



	}


	void Tool::getItem(std::list<std::string> * list, int& len, const char* buffer) {
		std::smatch m;
		std::string ss(buffer);

		//std::regex Pala("[0-9.a-zA-ZñÑáéíóúÁÉÍÓÚäëïöüÄËÏÖÜ]+");
		std::regex Pala("[^,]+");
		
		while (std::regex_search(ss, m, Pala)) {
			
			for (int i = 0; i < m.size(); i++) {
			
				list->push_back(m[i].str());

			}

			ss = m.suffix().str();
		}

		
	
	}
	
	void Tool::getCommand(std::string w[], int& len, const char* buffer) {

		std::string subject(buffer);
		std::smatch match;
		std::regex re("(\\$(\\w+):(\\w+)(\\+\\w+)?(?:=(.+)?)?)");
		len = 0;
		if (std::regex_search(subject, match, re)) {

			for (int i = 0; i < match.size(); i++) {
				w[i] = match[i].str();
				//cout << "W es : " << i << ", " << w[i] << endl;
				len++;
			}

			subject = match.suffix().str();
		}
	}
	void Tool::getSendCommand(std::string w[], int& len, const char* buffer) {

	std::string subject(buffer);
		std::smatch match;
		std::regex re("(\\$(\\w+)\\+(\\w+)(\\+\\w+)?(?:=(.+)?)?)");
		len = 0;
		if (std::regex_search(subject, match, re)) {

			for (int i = 0; i < match.size(); i++) {
				w[i] = match[i].str();
				//cout << "W es : " << i << ", " << w[i] << endl;
				len++;
			}

			subject = match.suffix().str();
		}
	}
	void Tool::getTracking(std::string w[], int& len, const char* buffer) {
		std::string subject(buffer);
		std::smatch match;
		std::regex re("(\\$(RP):(\\w+),(.+))");
		len = 0;
		if (std::regex_search(subject, match, re)) {

			for (int i = 0; i < match.size(); i++) {
				w[i] = match[i].str();
				//cout << "ZZZZ es : " << i << ", " << w[i] << endl;
				len++;
			}

			subject = match.suffix().str();
		}
	}

	template <class Container>
	void Tool::split2(const std::string& str, Container& cont, char delim) {
		std::stringstream ss(str);
		std::string token;
		while (std::getline(ss, token, delim)) {
			cont.push_back(token);
		}
	}


}

