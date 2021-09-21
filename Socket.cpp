#include "Socket.h"


namespace GT {
	Socket::Socket(SocketInfo pInfo):info(pInfo) {
		maxClients = info.maxClients;
	}
	void Socket::start() {
		//puts("startUp");
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
			printf("Failed. Error Code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
		//puts("createSocket");
		if ((master = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
			printf("Could not create socket : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
		//puts("Bing");
		//Prepare the sockaddr_in structure
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = INADDR_ANY;
		server.sin_port = htons(info.port);

		//Bind
		if (bind(master, (struct sockaddr*) & server, sizeof(server)) == SOCKET_ERROR) {
			printf("Bind failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}

		startListen();
	}

	void Socket::stop() {
	}

	int Socket::disconect(SOCKET client) {
		//shutdown(client, 2);
		
		for (int i = 0; i < maxClients; i++) {
			if (clients[i] == client) {
				std::cout << " CLOSE SOCKET " << client << "\n\n";
				closesocket(client);
				clients[i] = 0;
				return 1;
			}
		}
		return 0;
	}

	void Socket::startListen() {
		int h = listen(master, 3);

		//Accept and incoming connection
		//puts("Waiting for incoming connections...");

		int addrlen = sizeof(struct sockaddr_in);
		int activity;
		int valread;
		int i;
		fd_set readfds;
		//fd_set writefds;
		SOCKET s;

		int MAXRECV = 2048;

		char* buffer = (char*)malloc((MAXRECV + 1) * sizeof(char));

		for (i = 0; i < maxClients; i++) {
			
			clients[i] = 0;
		}
		
		while (TRUE) {
			FD_ZERO(&readfds);//clear the socket fd set
			FD_SET(master, &readfds);//add master socket to fd set
			//memset(&buffer, 0, MAXRECV);//clear the buffer
			//add child sockets to fd set
			for (i = 0; i < maxClients; i++) {
				s = clients[i];
				if (s > 0) {
					FD_SET(s, &readfds);
				}
			}
			//wait for an activity on any of the sockets, timeout is NULL , so wait indefinitely
		
			activity = select(0, &readfds, NULL, NULL, NULL);
			
			if (activity == SOCKET_ERROR) {
				printf("select call failed with error code : %d", WSAGetLastError());
				exit(EXIT_FAILURE);
			}
			//If something happened on the master socket , then its an incoming connection
			if (FD_ISSET(master, &readfds)) {
				if ((tSocket = accept(master, (struct sockaddr*) & address, (int*)&addrlen)) < 0) {
					puts("error");
					perror("accept");
					exit(EXIT_FAILURE);
				}
			
				//inform user of socket number - used in send and receive commands
				//printf("New connection , socket fd is %d , ip is : %s , port : %d (?)\n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
				//add new socket to array of sockets
				
				for (i = 0; i < maxClients; i++) {
				
					if (clients[i] == 0) {
						ConnInfo Info;
						Info.master = master;
						Info.client = tSocket;
						Info.clients = clients;
						Info.index = i;
						Info.maxClients = maxClients;
						Info.address = inet_ntoa(address.sin_addr);
						Info.port = ntohs(address.sin_port);
						Info.tag = (char*)"yanny";
						Info.clock = clock();
						clients[i] = tSocket;
						//CallConection(Info);
						//std::cout << "new conecction " << Info.address  << "\n\n";

						//CallConection(master, new_socket, clients, i, maxClients, info);
						//printf("Adding to list of sockets at index %d \n", i);
						onConnect(Info);
						break;
					}
				}
			}

			//else its some IO operation on some other socket :)
			for (i = 0; i < maxClients; i++) {
				s = clients[i];
				//if client presend in read sockets             
				if (FD_ISSET(s, &readfds)) {
					//get details of the client
					getpeername(s, (struct sockaddr*) & address, (int*)&addrlen);

					//Check if it was for closing , and also read the incoming message
					//recv does not place a null terminator at the end of the string (whilst printf %s assumes there is one).
					//memset(&buffer, 0, MAXRECV);//clear the buffer
					valread = recv(s, buffer, MAXRECV, 0);
					if (valread == SOCKET_ERROR) {
						int error_code = WSAGetLastError();
						if (error_code == WSAECONNRESET) {
							//Somebody disconnected , get his details and print
							printf(ANSI_COLOR_RED "1. Host disconnected unexpectedly, ip: %s, port: %d \n" ANSI_COLOR_RESET, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
							//Close the socket and mark as 0 in list for reuse
							closesocket(s);
							clients[i] = 0;
						} else {
							//printf("buffer %s", buffer);
							printf("\nrecv failed with error code: [ %d ]\n", error_code);
						}
						//CallClientError(master, s, buffer, valread, i, WSAGetLastError());
						ConnInfo Info;
						Info.master = master;
						Info.client = s;
						Info.clients = clients;
						Info.index = i;
						Info.maxClients = maxClients;
						Info.address = inet_ntoa(address.sin_addr);
						Info.port = ntohs(address.sin_port);
						Info.tag = (char*)"error UNO";
						Info.buffer = buffer;
						Info.valread = valread;
						Info.error = WSAGetLastError();
						//CallClose(Info);
						onClose(Info);
					}
					if (valread == 0) {
						//Somebody disconnected , get his details and print
						printf("2. Host disconnected , ip %s , port %d \n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

						//Close the socket and mark as 0 in list for reuse
						ConnInfo Info;
						Info.master = master;
						Info.client = s;
						Info.clients = clients;
						Info.index = i;
						Info.maxClients = maxClients;
						Info.address = inet_ntoa(address.sin_addr);
						Info.port = ntohs(address.sin_port);
						Info.tag = (char*)"ERROR dos";
						Info.buffer = buffer;
						Info.valread = valread;
						Info.error = WSAGetLastError();
						//CallClose(Info);
						onClose(Info);
						//CallClientError(master, s, buffer, valread, i, WSAGetLastError());
						closesocket(s);
						clients[i] = 0;
					} else {
						//Echo back the message that came in
						//add null character, if you want to use with printf/puts or other string handling functions
						buffer[valread] = '\0';

						ConnInfo Info;
						Info.master = master;
						Info.client = s;
						Info.clients = clients;
						Info.index = i;
						//Info.maxClients = maxClients;
						Info.address = inet_ntoa(address.sin_addr);
						Info.port = ntohs(address.sin_port);
						Info.tag = (char*)"GT";
						Info.buffer = buffer;
						Info.valread = valread;
						Info.clock = clock();

						//std::cout << "\n\n**** reloj: " << Info.clock << std::endl;
						//CallMessage(Info);
						onMessage(Info);
						
					}
				}
			}
		}
		//CloseHandle(hClientThread);
		closesocket(s);
		WSACleanup();

	}
}