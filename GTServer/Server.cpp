#include <stdio.h>
#include "Server.h"

using namespace rapidjson;
using namespace std;
namespace GT {

	Server::Server(SocketInfo pInfo):Socket(pInfo),debug(false), db(nullptr) {
	}

	bool Server::init(AppConfig pConfig) {


		std::string result[20];
		int len;
		int nLine = 0;

		const char* x = "$OK:TRACK=1,1,1,1,1";
		Tool::getCommand(result, len, x);


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

		printf("coneccting client ID %d\n", Info.client);
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
		

		if (clients.count(Info.client)<=0) {
			printf("error client dead!!!!\n");
			return;
		}
		printf("recibiendo: %i, tag: %s, buffer: %s\n", Info.client,Info.tag, Info.buffer);
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
			return;
		}


		IdHeader* h = getMsgHeader(Info.buffer);
		printf("Header %d, Type %d\n", h->header, h->type);
		unsigned short type = getHeader(Info);
		
		printf("Info.client %d, type: %d\n", Info.client, clients[Info.client].type);

		if (clients[Info.client].type == 2) {
			printf("Info.Device %s, %d, version: %d\n", clients[Info.client].device_id, clients[Info.client].id, clients[Info.client].version_id);
			deviceMessage(Info);

			//return;
		}
		evalMessage(Info, Info.buffer);
	}

	void Server::onClose(ConnInfo Info) {
		printf("ERROR ESTOY DESCONECTANDO (%i): %s client: %d\n", Info.error, Info.tag, Info.client);
		
		for (std::map<SOCKET, GTClient >::iterator it = clients.begin(); it != clients.end(); it++) {
			printf("there are client..(%i, %i) \n", it->first, it->second.socket);
		}

		
		clients.erase(Info.client);

		for (std::map<SOCKET, GTClient >::iterator it = clients.begin(); it != clients.end(); it++) {
			printf("there are at now client..(%i, %i) \n", it->first, it->second.socket);
		}
	}


	
	bool Server::isSyncMsg(ConnInfo Info) {


		SyncMsg* sync_msg = (SyncMsg*)Info.buffer;

		char id[12];

		if (db->isVersion(sync_msg->Keep_Alive_Header)) {
			printf(ANSI_COLOR_CYAN "---> verification of sync (%lu)..(%d).\n" ANSI_COLOR_RESET, sync_msg->Keep_Alive_Device_ID, sync_msg->Keep_Alive_Header);
			//puts(sync_msg->Keep_Alive_Device_ID));

			sprintf(id, "%lu", sync_msg->Keep_Alive_Device_ID);
			//printf("\nasync %d\n", sync_msg->Keep_Alive_Device_ID);

			
			mDevices[id] = clients[Info.client];

			if (mDevices[id].type != 2) {

				cout << "VERIFICANDO CONEXION" << endl;
				clients[Info.client].type = 2;
				mDevices[id].type = 2;
				strcpy(mDevices[id].device_id, (const char*)id);
				InfoClient cInfo = db->getInfoClient(id);
				mDevices[id].id = cInfo.unit_id;
				mDevices[id].version_id = cInfo.version_id;

				clients[Info.client].id = cInfo.unit_id;
				clients[Info.client].version_id = cInfo.version_id;
				strcpy(clients[Info.client].device_id, (const char*)id);
			}

			
			

			//db->saveEvent("88", 4);
			printf(ANSI_COLOR_RED "Save Event from: (%s) %d, version: %d \n" ANSI_COLOR_RESET, id, mDevices[id].id, mDevices[id].version_id);

			send(Info.client, Info.buffer, Info.valread, 0);// return the sycm message

			//const char* buf = "$WP+VER=0000,?";
			//send(Info.client, buf, strlen(buf), 0);
			return true;
		}
		


		return false;
	}

	unsigned short Server::getHeader(ConnInfo Info) {
		IdHeader* header = (IdHeader*)Info.buffer;
		std:string command = "";

		if (header->header == 10020) {
			RCommand * r = (RCommand*)Info.buffer;
			if (header->type == 1) {
				std::cout << " mensaje " << r->message << " unit " << r->unit << endl;

				//send(Info.client, "JEJE", 5, 0);
				send(mDevices[r->unit].socket, r->message, strlen(r->message), 0);
			}
			
			return 0;

		}

		if (header->header == 10010) {
			CMDMsg* msg = (CMDMsg*)Info.buffer;
			cout << "type msg: " << msg->type << endl;
			cout << "Unit ID: " << msg->unitId << endl;
			if (msg->type == 1 || msg->type == 2) {
				command = db->createCommand(msg, msg->unitId, msg->cmdId);
			}
			if (msg->type == 3) {
				command = db->loadCommand(msg, msg->cmdId);
			}
			if (mDevices[msg->deviceName].type == 2) {
				cout << "Sending command!!!"<< endl;
			} else {
				cout << "Unit disconected!!!" << endl;
			}
			send(Info.client, command.c_str(), strlen(command.c_str()), 0);
		}
		printf("header: %d\n", header->header);
		

		return header->type;
	}

	IdHeader * Server::getMsgHeader(const char* msg) {
		IdHeader * header = (IdHeader*)msg;
		return header;
	}

	
	
	bool Server::evalMessage(ConnInfo Info, const char* message) {

		printf("evalMessage: %s\n", message);

		Command* cmd = (Command*)message;

		//printf("token. %i \n", cmd->token);
		//printf("id. %s \n", cmd->id);
		//printf("message. %s \n", cmd->message);

		if (cmd->token == 9) {
			DeviceMSG * m2 = (DeviceMSG*) cmd->message;
			//m2->id[sizeof(m2->id)] = '\0';
			printf("m2 id. %d: %s \n", sizeof(m2->id), m2->id);
			printf("m2 id. %d: %s \n", sizeof(m2->message), m2->message);

			if (mDevices.count(m2->id)>0) {
				printf("si es igual.\n");
				send(mDevices[m2->id].socket, m2->message, strlen(m2->message), 0);
			}
				
			for (std::map<string, GTClient>::iterator it = mDevices.begin(); it != mDevices.end(); it++) {
				if (m2->id == it->first.c_str()) {
					printf("si es igual.\n");
				}

					
				printf("there are a client..%s..\n", it->first.c_str() );
					
					
			}



		}


		switch (cmd->token) {
		case '*':
			puts("* asterisco");
			//shutdown(Info.client, 0);
			disconect(Info.client);
			break;
		case '$':
			puts("$ dolar");
			//system("cls");
			break;
		case '%':
			puts("% porcentaje");
			break;
		case '-':
			puts("- return");
			send(Info.client, Info.buffer, Info.valread, 0);
			break;
		default:
			puts(message);
			;
			break;

		}

		return false;
	}

	bool Server::deviceMessage(ConnInfo Info) {

		printf("esto es un device message\n");


		//std::string string = (char*)message;
		std::stringstream ss((char*)Info.buffer);
		std::string to;
		int i = 0;

		std::string result[20];
		int len;
		int nLine = 0;

		if (Info.buffer != NULL) {
			while (std::getline(ss, to)) {//, '\n'
				Tool::getCommand(result, len, to.c_str());
				
				nLine++;


				printf(ANSI_COLOR_CYAN "linea: %d\n", nLine);
				
				
				if (len > 0) {
					cout << "es un COMANDO" << endl;
					CommandResult  rCommand = {
						result[2],
						result[3],
						result[4],
						result[5]

					};
					db->deviceConfig(clients[Info.client].device_id, & rCommand);
					db->evalPending(clients[Info.client].device_id, &rCommand);
				} else {
					cout << "es un track" << endl;
					db->saveTrack(clients[Info.client].device_id, to.c_str());
				}
				std::cout << "LINE: " << to.c_str() << endl;
				printf("" ANSI_COLOR_RESET);


				//printf("id %d, %d .- %s\n", clients[Info.client].device_id, i++, to.c_str());

				
				

				//_CallMsgReceived2(master, client, (char*)to.c_str(), valread, index);
			}
		}

		return false;
	}
}