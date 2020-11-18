#include "Tool.h"




using namespace std;



namespace GT {
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
	void Tool::getItem(std::string w[], int& len, const char* buffer) {

		std::smatch m;
		std::string ss(buffer);

		//std::regex Pala("[0-9.a-zA-ZñÑáéíóúÁÉÍÓÚäëïöüÄËÏÖÜ]+");
		std::regex Pala("[^,]+");
		len = 0;
		while (std::regex_search(ss, m, Pala)) {

			for (int i = 0; i < m.size(); i++) {
				w[len++] = m[i].str();
			}

			ss = m.suffix().str();

		}

	}
	void Tool::getCommand(std::string w[], int& len, const char* buffer) {

		std:string subject(buffer);
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
	void Tool::getTracking(std::string w[], int& len, const char* buffer) {
		std:string subject(buffer);
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

