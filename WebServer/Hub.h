#pragma once
#include "SocketClient.h"
#include <iostream>

namespace GT {
	class Hub :	public SocketClient {

	public:

		
		Hub(CSInfo pInfo);
		void onConect();
		void onReceive(char* buffer, size_t size);

		void (*CallConection)(CSInfo Info);
		void (*CallMessage)(CSInfo Info);
		void (*CallClose)(CSInfo Info);
		void (*callReceive)(void * app, char* buffer, size_t size);
		void * appData;
		std::string nn = "yanny";
	private:
		CSInfo pInfo;
	
	};
}
