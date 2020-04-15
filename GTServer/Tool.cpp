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

		//std::regex Pala("[0-9.a-zA-Z����������������������]+");
		std::regex Pala("[0-9.a-zA-Z]+");
		len = 0;
		while (std::regex_search(ss, m, Pala)) {

			for (int i = 0; i < m.size(); i++) {
				w[len++] = m[i].str();
			}

			ss = m.suffix().str();

		}

	}
}
