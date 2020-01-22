#pragma once
#include <iostream>
#include "CSocket.h"
BOOL WINAPI keyboard(LPVOID param);
namespace GT {
	class Client:public CSocket {
	public:
		Client(CSInfo pInfo);

		virtual void onConect();
	private:
		HANDLE hClientThread;
		DWORD dwThreadId;
	};


}