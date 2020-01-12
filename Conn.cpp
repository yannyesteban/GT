#include "Conn.h"
//#include <stdio.h>
namespace GT {
	


	Conn::Conn(SocketInfo pInfo):Socket(pInfo) {

		CallConection = _CallConection;
		CallMessage = _CallMessage;
		CallClose = _CallClose;
	}

	void Conn::_CallConection(ConnInfo Info) {
		puts("_CallConection");
	}

	void Conn::_CallMessage(ConnInfo Info) {
		puts("_CallMessage");
		Color::set(Color::red);
		printf(ANSI_COLOR_CYAN "return sync message...!\n[%s]\n" ANSI_COLOR_RESET, Info.buffer);

		send(Info.client, Info.buffer, Info.valread, 0);// return the sycm message
	}

	void Conn::_CallClose(ConnInfo Info) {
		puts("_CallClose");
	}

}