#pragma once
#include <iostream>
#include "SocketClient.h"
BOOL WINAPI keyboard(LPVOID param);
namespace GT {
	class Client:public SocketClient {
	public:
		Client(CSInfo pInfo);

		virtual void onConect();
	private:
		HANDLE hClientThread;
		DWORD dwThreadId;
	};


}