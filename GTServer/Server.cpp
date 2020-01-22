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
		//db->loadClients();
		db->loadFormats();
		db->saveTrack("", 1, 21, "");
		return true;
	}

	void Server::onConnect(ConnInfo Info) {
		if (clients.count(Info.client) > 0) {
			puts("Receiving...");
		} else {
			puts("New Client...");
			clients[Info.client].status = 1;
			clients[Info.client].socket = Info.client;
			clients[Info.client].type = 2;
			strcpy(clients[Info.client].device_id, "unknow");
		}
	}

	void Server::onMessage(ConnInfo Info) {
		printf("recibiendo: %s\n", Info.buffer);

		if (isSyncMsg(Info)) {

		}
		evalMessage(Info.buffer);
	}

	void Server::onClose(ConnInfo Info) {
	}


	
	bool Server::isSyncMsg(ConnInfo Info) {
		SyncMsg* sync_msg = (SyncMsg*)Info.buffer;

		char id[12];

		printf(ANSI_COLOR_CYAN "---> verification of sync (%lu)..." ANSI_COLOR_RESET, sync_msg->Keep_Alive_Device_ID);
		//puts(sync_msg->Keep_Alive_Device_ID));

		if (db->isVersion(sync_msg->Keep_Alive_Header)) {
			sprintf(id, "%lu", sync_msg->Keep_Alive_Device_ID);
			return true;
		}
		/*for (int i = 0; i < versions.n; i++) {
			if (sync_msg->Keep_Alive_Header == versions.e[i]) {
				sprintf(id, "%lu", sync_msg->Keep_Alive_Device_ID);
				return true;
			}
		}*/
		return false;
	}
	bool Server::evalMessage(const char* message) {

		Command* cmd = (Command*)message;
		switch (cmd->token) {
		case '*':
			puts("* asterisco");
			break;
		case '$':
			puts("$ dolar");
			break;
		case '%':
			puts("% porcentaje");
			break;
		default:
			puts("otra cosa");

			//std::string string = (char*)message;
			std::stringstream ss((char*)"hello \npedro\npor su casa\n");
			std::string to;
			int i = 0;




			if (message != NULL) {
				while (std::getline(ss, to)) {//, '\n'

					printf("\n%d .- %s", i++, to.c_str());

					//_CallMsgReceived2(master, client, (char*)to.c_str(), valread, index);
				}
			}


			break;

		}

		return false;
	}
}