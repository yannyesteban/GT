#include "Hub.h"


namespace GT {
	Hub::Hub(CSInfo pInfo) :SocketClient(pInfo) {}

	void Hub::onConect() {
		
		
		//strcpy(info.name, "zeta!");
		CallConection(info);

	}

	void Hub::onReceive(char* buffer, size_t size) {
		

		callReceive(appData, buffer, size);

	}

}