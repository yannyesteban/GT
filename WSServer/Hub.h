#pragma once
#include "SocketClient.h"


namespace GT {
	class Hub :	public SocketClient {
	public:
		Hub(CSInfo pInfo);
		void onConect();
		void onReceive(char* buffer, size_t size);

		void (*CallConection)(CSInfo Info);
		void (*CallMessage)(CSInfo Info);
		void (*CallClose)(CSInfo Info);
		void (*test)(char* buffer, size_t size);

	private:
		
	
	};
}
