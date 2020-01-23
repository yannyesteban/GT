#pragma once
#include "SocketClient.h"


namespace GT {
	class Hub :	public SocketClient {
	public:
		Hub(CSInfo pInfo);
		void onConect();
	private:
		
	
	};
}
