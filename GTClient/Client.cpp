#include "Client.h"
#include "Types.h"
namespace GT {
	Client::Client(CSInfo pInfo) :SocketClient(pInfo) {}
	void Client::onConect() {

		hClientThread = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)keyboard,
			this,
			0,
			&dwThreadId);
	}



}

BOOL __stdcall keyboard(LPVOID param) {
	GT::Client* Info = (GT::Client*)param;

	char message[255] = "";
	char buff[255];
	int iResult;
	//printf("que %s", "hey");
	while (strcmp(message, "*exit")) {
		
		//fflush(stdin);
		std::cout << "-> ";
		std::cin.getline(message, 255, '\n');


		//system("cls");
		//fflush(stdin);
		//std::cin.ignore(10, '\n');
		//std::cin.ignore(std::cin.rdbuf()->in_avail());
		//printf("> ");
		//scanf_s("%s!", message,255);
		//scanf_s("%200[^\n]", message, 200);
		//fflush(stdin);
		//system("pause");
		//printf("mensaje enviado %s // length[%d]\n", message, (int)strlen(message));
		std::cin.clear();
		//continue;
		//message[strlen(message)] = '\0';

		//char MM[] = "*===Multi Hilo===";
		//memset(&sendbuf, 0, sizeof(sendbuf));//clear the buffer
		//strcpy(sendbuf, buffer);
		//iResult = send(ConnectSocket,  mensajes[i], 10, 0);
		

		GT::RCommand r = {
			10020,
			1,
			"pepe",
			"$WP+VER=0000,?",
			"2012000750",
			4737
		};


		printf(".unit [%d] %s...\n", strlen(r.user), r.user);

		memcpy(message, &r, sizeof(r));

		iResult = send(Info->getHost(), message, (int)sizeof(message), 0);

		GT::Command xx = {
			9,
			"201200422",
			"201200422aaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbb",


		};
		GT::DeviceMSG m2 = {
			"2012000750",
			"$WP+COMMTYPE=0000,?"//"$WP+TRACK=0000,? "

		};
		message[strlen(message)+1] = '\0';
		memcpy(m2.message, message, strlen(message)+1);
		printf("==message-> %s\n", m2.message);
		printf("..message-> %s\n", xx.message);
		memcpy(xx.message, &m2, sizeof(m2));

		printf("TOKEN-> %d\n", xx.token);
		printf("message-> %s\n", xx.message);
		printf("id-> %s\n", xx.id);
		//char  s[] = "impresionante";
		//memcpy(xx.message, &s, sizeof(s));
		
		
		memcpy(buff, &xx, sizeof(xx));
		//Info.valread = sizeof(xx);



		//iResult = send(Info->getHost(), message, (int)strlen(message), 0);
		iResult = send(Info->getHost(), buff, (int)sizeof(buff), 0);
		if (iResult == SOCKET_ERROR) {
			printf("send failed with error-> %d\n", WSAGetLastError());
			closesocket(Info->getHost());
			WSACleanup();
			exit(0);
			return false;
		}
		printf("   => Ok, length[%d]\n", (int)strlen(message));

	}
	system("cls");
	printf("**************\n\n");

	closesocket(Info->getHost());
	WSACleanup();
	exit(0);
	return false;
}
