#include "Client.h"
namespace GT {
	Client::Client(CSInfo pInfo) :CSocket(pInfo) {}
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
		iResult = send(Info->getHost(), message, (int)strlen(message), 0);
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
