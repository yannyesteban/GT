#include <stdio.h>
#include "Server.h"


using namespace std;
namespace GT {

	Server::Server(SocketInfo pInfo):Socket(pInfo),debug(false), db(nullptr) {
	}

	bool Server::init(AppConfig pConfig) {
		//config = pConfig;
		pConfig.db.debug = pConfig.debug;
		db = new DB(pConfig.db);
		db->connect();
		db->loadProtocols();
		db->loadVersions();
		db->loadClients();
		db->loadFormats();
		db->saveTrack("", 1, 21, "");
		return true;
	}

	void Server::onConnect(ConnInfo Info) {
		if (clients.count(Info.client) > 0) {
			//puts("Receiving...");
		} else {
			puts("New Client...");
			clients[Info.client].status = 1;
			clients[Info.client].socket = Info.client;
			clients[Info.client].type = 1;
			strcpy(clients[Info.client].device_id, "unknow");
		}
	}

	void Server::onMessage(ConnInfo Info) {
		printf("recibiendo: %s\n", Info.buffer);
		/*
		SyncMsg xx = {
			63738,
			283,
			2012000422,

		};
		memcpy(Info.buffer, &xx, sizeof(xx));
		Info.valread = sizeof(xx);

		*/
		//Info.buffer = (void *)xx;

		if (isSyncMsg(Info)) {
			//return;
		}

		if (clients[Info.client].type == 2) {
			deviceMessage(Info);

			//return;
		}
		evalMessage(Info, Info.buffer);
	}

	void Server::onClose(ConnInfo Info) {
	}


	
	bool Server::isSyncMsg(ConnInfo Info) {

		


		SyncMsg* sync_msg = (SyncMsg*)Info.buffer;

		char id[12];

		

		if (db->isVersion(sync_msg->Keep_Alive_Header)) {
			printf(ANSI_COLOR_CYAN "---> verification of sync (%lu)..(%d).\n" ANSI_COLOR_RESET, sync_msg->Keep_Alive_Device_ID, sync_msg->Keep_Alive_Header);
			//puts(sync_msg->Keep_Alive_Device_ID));

			sprintf(id, "%lu", sync_msg->Keep_Alive_Device_ID);
			printf("\nasync %d\n", sync_msg->Keep_Alive_Device_ID);


			mDevices[id] = clients[Info.client];
			mDevices[id].type = 2;
			strcpy(mDevices[id].device_id, (const char*)id);
			send(Info.client, Info.buffer, Info.valread, 0);// return the sycm message
			return true;
		}
		


		return false;
	}
	
	bool Server::evalMessage(ConnInfo Info, const char* message) {

		Command* cmd = (Command*)message;
		switch (cmd->token) {
		case '*':
			puts("* asterisco");
			//shutdown(Info.client, 0);
			disconect(Info.client);
			break;
		case '$':
			puts("$ dolar");
			system("cls");
			break;
		case '%':
			puts("% porcentaje");
			break;
		case '-':
			puts("- return");
			send(Info.client, Info.buffer, Info.valread, 0);
			break;
		default:
			puts("otra cosa");
			;
			break;

		}

		return false;
	}

	bool Server::deviceMessage(ConnInfo Info) {

		printf("esto es un device message\n");


		//std::string string = (char*)message;
		std::stringstream ss((char*)"hello \npedro\npor su casa\n");
		std::string to;
		int i = 0;




		if (Info.buffer != NULL) {
			while (std::getline(ss, to)) {//, '\n'
				printf("%d .- %s\n", i++, to.c_str());

				db->saveTrack(clients[Info.client].device_id, to.c_str());
				

				//_CallMsgReceived2(master, client, (char*)to.c_str(), valread, index);
			}
		}

		return false;
	}
}