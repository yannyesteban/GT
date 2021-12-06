#pragma once
#include <Socket.h>
#include <list>
#include <string>
#include <map>

namespace GT {

	class iStartek	{
	public:
		bool isSynch(ConnInfo Info);

		std::string getTracking(ConnInfo Info);

		std::list<std::string> getTrackingList(const std::string s);

		std::map<std::string, std::string> getEventData(const std::string s);

	private:

	};

}
