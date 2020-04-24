#include "Hub.h"


namespace GT {
	Hub::Hub(CSInfo pInfo) :SocketClient(pInfo) {}

	void Hub::onConect() {
	}

	void Hub::onReceive(char* buffer, size_t size) {
		printf("HUB principal Message: %s\nBytes received: %d\n> ", buffer, size);

		callReceive(appData, buffer, size);
	}

}