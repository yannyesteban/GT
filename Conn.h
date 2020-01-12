#pragma once
#include "Socket.h"
#include "Color.h"
namespace GT {
	class Conn : public Socket {
	public:

		Conn(SocketInfo pInfo);
		

		static void _CallConection(ConnInfo Info);
		static void _CallMessage(ConnInfo Info);
		static void _CallClose(ConnInfo Info);
		
	private:

	};

}