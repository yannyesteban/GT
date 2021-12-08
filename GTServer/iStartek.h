#pragma once
#include <Socket.h>
#include <list>
#include <string>
#include <map>

namespace GT {
	typedef struct {
		char token[3];
		unsigned short packLength;

	} iStartekHeader;
	class iStartek {
	public:
		bool isSynch(ConnInfo Info);

		std::string getTracking(ConnInfo Info);

		std::string getTracking(const std::string s);

		std::list<std::string> getTrackingList(const std::string s);

		std::map<std::string, std::string> getEventData(const std::string s);

		bool isMe(ConnInfo Info);
	

	private:

	};

}
