#include <stdio.h>
#include "Server.h"

using namespace rapidjson;
using namespace std;
namespace GT {
	
	
	void runTimer() {
		setlocale(LC_CTYPE, "Spanish");
		
		while (true) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
			auto t = std::time(nullptr);
			auto tm = *std::localtime(&t);
			//std::cout << std::put_time(&tm, "%d/%m/%Y %H:%M:%S") << std::endl;
			//std::cout << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << std::endl;
	
	
	

			//double time = (double(t1 - t0) / CLOCKS_PER_SEC);
			//std::cout << "Execution Time: " << t1 << std::endl;
		}
	}

	void runTimeOut(std::map < SOCKET, GTClient> * clients, Server* s, int keepAliveTime) {
		clock_t mClock;
		
		

		std::cout << s->keepAliveTime << std::endl;
		while (true) {
			
			//s->db->updateUnitConnected();
			std::cout << "My Thread is " << std::this_thread::get_id() << "\n\n";
			
			std::this_thread::sleep_for(std::chrono::seconds(10));
			
			
			mClock = clock();
			
			for (std::map<SOCKET, GTClient>::iterator it = clients->begin(); it != clients->end(); ++it) {
				
				double diffClock = (double(mClock) - double(it->second.clock)) / CLOCKS_PER_SEC;
				//cout << "CLOCK -> " << diffClock << " type :" << it->second.type << endl;
				if (diffClock > 160 && it->second.type ==2) {
					cout << "cerrando el socket del cliente " << it->second.socket << endl;
					//s->disconect(it->second.socket);
				}
				//printf("%12s", it->first.c_str());
				//printf("%10s", it->second.device_id);
				//printf("%10d\n", it->second.id);

				//std::cout << "---- one clock " << it->second.clock;
			}


		}

	}

	void runPending(std::map<string, GTClient > * mDevices, DB* db) {
		while (true) {
			std::this_thread::sleep_for(std::chrono::seconds(10));
			
			std::vector<PendingCommand> v1;
			std::vector<std::string> v2;

			for (std::map<std::string, GTClient>::iterator it = mDevices->begin(); it != mDevices->end(); ++it) {
				//printf("%12s", it->first.c_str());
				//printf("%10s", it->second.device_id);
				//printf("%10d\n", it->second.id);

				v2.push_back(std::to_string(it->second.id));
			}

			if (!db->getPendingCommand(&v1, v2)) {
				return;
			}
				
			for (std::vector<PendingCommand>::iterator it = v1.begin(); it != v1.end(); ++it) {

				//std::cout << Color::_yellow() << "NAME " << it->name << "\n\n" << Color::_reset();
				auto x = mDevices->find(it->name);
				if (x == mDevices->end()) {
					//std::cout << "*NOT Found " << it->name << "\n\n";
				} else {
					//std::cout << "Found " << x->first << "\n\n";
					send(x->second.socket, it->command.c_str(), strlen(it->command.c_str()), 0);
					std::this_thread::sleep_for(std::chrono::seconds(2));
					std::cout << Color::_green() << "Sending Pending Command: " << it->command.c_str()
						<< Color::_reset() << " to: "
						<< x->second.device_id << " (" << "r->unit" << ")\n";
					// found
				}
				//std::cout << it->name << "   ----       (" << it->command << ")\n";
				//printf("%20s", *it2);

			}


			//std::cout << "pause of " << 4888 << " seconds ended\n";
		}
		
	}
	
	Server::Server(SocketInfo pInfo):Socket(pInfo),debug(false), db(nullptr) {
		mClock = clock();
	}

	bool Server::init(AppConfig pConfig) {

		/*
		time_t now;
		struct tm newyear;
		double seconds;

		time(&now);  // get current time; same as: now = time(NULL)  

		newyear = *localtime(&now);

		newyear.tm_hour = 0; newyear.tm_min = 0; newyear.tm_sec = 0;
		newyear.tm_mon = 0;  newyear.tm_mday = 1;

		seconds = difftime(now, mktime(&newyear));

		printf("%.f seconds since new year in the current timezone.\n", seconds);
		*/
		/*
		std::string result[20];
		int len;
		int nLine = 0;

		const char* x = "$OK:TRACK=1,1,1,1,1";
		Tool::getCommand(result, len, x);

		*/
		//config = pConfig;
		
		keepAliveTime = pConfig.keep_alive;
		pConfig.db.debug = pConfig.debug;
		db = new DB(pConfig.db);
		db->connect();
		
		//db->saveTrack("", 1, 21, "");


		webcar = new WC::Webcar("webcar.json");
		
		//WC::TrackParam P({ 1, 2087, "200210001","2020-10-13 06:51:00", 10.2121, 60.121,80,15 });
		//webcar->evalTrack(&P);
		//system("pause");

		//std::thread* first = new std::thread(runPending, &mDevices, std::ref(db));
		
		
		
		/* nothing for now*/
		//std::thread* first = new std::thread(runTimer);

		//first->join();
		InfoPending infoPending;
		db->getInfoPending(274,&infoPending);
		//std::thread* threadTimeOut = new std::thread(runTimeOut, &clients, this, keepAliveTime);
		return true;
	}

	void Server::onConnect(ConnInfo Info) {

		//printf("coneccting client ID %d\n", Info.client);
		if (clients.count(Info.client) > 0) {
			//
		} else {
			
	
			std::cout << Color::_green()<< "\nNew Client: Id = " << Info.client << Color::_reset() << std::endl;
	
			clients[Info.client].status = 1;
			clients[Info.client].socket = Info.client;
			clients[Info.client].type = 1;
			strcpy(clients[Info.client].device_id, "unknow");

			strcpy_s(clients[Info.client].name, sizeof(clients[Info.client].name), "nameless");
			//strcpy_s(clients[Info.client].address, sizeof(clients[Info.client].name), "nameless");

			clients[Info.client].address = Info.address;
			
		}
	}
	
	void Server::onMessage(ConnInfo Info) {
		//std::cout << "Main ? My Thread is " << std::this_thread::get_id() << "\n\n";
		
		if (clients.count(Info.client) <= 0) {
			std::cout << Color::_red() << Color::bwhite() << "\nClient Dead: Id = " << Info.client << Color::_reset() << std::endl;
			//printf("error client dead!!!!\n");
			return;
		}
		//printf("recibiendo: %i, tag: %s, buffer: %s\n", Info.client,Info.tag, Info.buffer);
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
		//printf("\nHeader %d, Type %d\n", h->header, h->type);
		unsigned short type = getHeader(Info);

		//printf("Info.client %d, type: %d\n", Info.client, clients[Info.client].type);

		if (clients[Info.client].type == 2) {
			//printf("Info.Device %s, %d, version: %d\n", clients[Info.client].device_id, clients[Info.client].id, clients[Info.client].version_id);
			deviceMessage(Info);

			//return;
		}
		evalMessage(Info, Info.buffer);
	}

	Server::~Server() {

		db->initStatus();
	}
	
	unsigned short Server::getHeader(ConnInfo Info) {
		IdHeader* header = (IdHeader*)Info.buffer;
		std::string command = "";
		
		//std::cout << "Header " << header->header << std::endl;
		/* message from Websocket Server == 10001 */
		if (header->header == 10001) {

			if (rClients.find(Info.client) != rClients.end()) {
				rClients.erase(Info.client);

				std::cout << "Existe Cliente WEBSOCKET "<<  "\n\n";
			}


			std::string str;

			RequestConnection* r = (RequestConnection*)Info.buffer;

			strcpy_s(clients[Info.client].name, sizeof(clients[Info.client].name), r->name);
			
			rClients[Info.client] = {};
			
			strcpy_s(rClients[Info.client].name, sizeof(rClients[Info.client].name), r->name);
			strcpy_s(rClients[Info.client].user, sizeof(rClients[Info.client].user), r->user);

			rClients[Info.client].type = 3;
			rClients[Info.client].status = 1;
			rClients[Info.client].socket = Info.client;

			cout << "Cliente Name: " << r->name << endl;
			return 0;

		}
		
		if (header->header == 10020) {
			RCommand* r = (RCommand*)Info.buffer;


			RCommand response;
			response.header = 10021;
			response.mode = r->mode;
			response.type = r->type;
			response.id = r->id;
			response.unitId = r->unitId;
			response.level = r->level;
			std::string str = r->message;
			char buffer[1024];
			strcpy_s(response.message, sizeof(response.message), str.c_str());
			//strcpy(response.message, str.c_str());

			str = r->unit;


			strcpy_s(response.unit, sizeof(response.unit), r->unit);
			//strcpy_s(response.unit, str.c_str());

			str = r->user;
			strcpy_s(response.user, sizeof(response.user), str.c_str());
			//strcpy_s(response.user, str.c_str());


			memcpy(buffer, &response, sizeof(response));
			send(Info.client, buffer, (int)sizeof(buffer), 0);


			send(mDevices[getUnitName(r->unitId)].socket, r->message, strlen(r->message), 0);
			
			std::cout << Color::_green() << "Sending: " << response.message 
				<<  Color::_reset() << " to: "
				<< mDevices[getUnitName(r->unitId)].device_id << " (" << r->unit << ")\n";



			time_t rawtime;
			struct tm* timeinfo;


			time(&rawtime);
			timeinfo = localtime(&rawtime);

			

			DBEvent event;
			event.unitId = r->unitId;
			event.eventId = 209;
			strftime(event.dateTime, sizeof(event.dateTime), "%F %T", timeinfo);
			strcpy(event.title, r->command);
			strcpy(event.user, r->user);
			strcpy_s(event.info, sizeof(r->message), r->message);
			//strcpy(event.info, "");
			db->insertEvent(&event);

			/*
			std::cout << "Header: " << response.header << std::endl;
			std::cout << "Message: " << response.message << std::endl;
			std::cout << "UnitId: " << response.unitId << std::endl;
			std::cout << "Mode: " << response.mode << std::endl;
			std::cout << "User: " << response.user << std::endl;
			std::cout << "Unit: " << response.unit << std::endl;
			*/


			return 0;

		}
		if (header->header == 10300) {
			RCommand* r = (RCommand*)Info.buffer;

			InfoPending infoPending;
			db->getInfoPending(r->id, &infoPending);

			std::cout << "Pending Id " << infoPending.command << std::endl;
			send(mDevices[getUnitName(infoPending.unitId)].socket, infoPending.command.c_str(), strlen(infoPending.command.c_str()), 0);
		}
		if (header->header == 10100) {
			RCommand* r = (RCommand*)Info.buffer;

			//std::cout << " W " << mDevices[getUnitName(r->unitId)].device_id << std::endl;

			if (mDevices[getUnitName(r->unitId)].socket <= 0) {
				//std::cout << " E R R O R \n";
				return 0;
			}

			RCommand response;
			response.header = 10021;
			response.mode = r->mode;
			response.type = r->type;
			response.id = r->id;
			response.unitId = r->unitId;
			response.level = r->level;
			std::string str = r->message;
			char buffer[1024];
			strcpy_s(response.message, sizeof(response.message), str.c_str());
			//strcpy(response.message, str.c_str());

			str = r->unit;


			strcpy_s(response.unit, sizeof(response.unit), r->unit);
			//strcpy_s(response.unit, str.c_str());

			str = r->user;
			strcpy_s(response.user, sizeof(response.user), str.c_str());
			//strcpy_s(response.user, str.c_str());


			memcpy(buffer, &response, sizeof(response));
			send(Info.client, buffer, (int)sizeof(buffer), 0);


			//send(mDevices[getUnitName(r->unitId)].socket, r->message, strlen(r->message), 0);

			std::cout << Color::_green() << "Sending: " << response.message
				<< Color::_reset() << " to: "
				<< mDevices[getUnitName(r->unitId)].device_id << " (" << r->unit << ")\n";
			
			std::cout << "Header: " << response.header << std::endl;
			std::cout << "Message: " << response.message << std::endl;
			std::cout << "UnitId: " << response.unitId << std::endl;
			std::cout << "Mode: " << response.mode << std::endl;
			std::cout << "User: " << response.user << std::endl;
			std::cout << "Unit: " << response.unit << std::endl;
			

			int value = disconect(mDevices[getUnitName(r->unitId)].socket);

			if (value > 0) {
				SOCKET oldSocket = mDevices[getUnitName(r->unitId)].socket;
				mDevices[getUnitName(r->unitId)].socket = 0;
				mDevices[getUnitName(r->unitId)].type = 0;
				db->setClientStatus(r->unitId, 0);

				mDevices.erase(getUnitName(r->unitId));
				clients.erase(oldSocket); 

				DBEvent event;
				event.unitId = r->unitId;
				event.eventId = 211;
				
				strcpy(event.title, "DISCONNECT");
				strcpy(event.user, r->user);
				strcpy_s(event.info, sizeof(r->message), r->message);
				//strcpy(event.info, "");
				db->insertEvent(&event);
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
				cout << "Sending command!!!" << endl;
			} else {
				cout << "Unit disconected!!!" << endl;
			}
			send(Info.client, command.c_str(), strlen(command.c_str()), 0);
		}
		//printf("header: %d\n", header->header);


		return header->type;
	}

	void Server::onClose(ConnInfo Info) {
		std::cout << " ON - CLOSE \n\n";

		clients[Info.client].device_id;
		RCommand resp;
		resp.unitId = clients[Info.client].id;
		resp.header = 0;
		resp.commandId = 0;
		resp.id = 0;
		resp.index = 0;
		resp.level = 0;
		
		resp.mode = 0;
		resp.type = 6;
		resp.typeMessage = ClientMsg::Disconnecting;
		strcpy(resp.message, "DISCONNECTED");
		strcpy(resp.unit, clients[Info.client].device_id);
		strcpy(resp.user, clients[Info.client].device_id);
		strcpy(resp.name, getClientName(clients[Info.client].id).c_str());

		time_t rawtime;
		struct tm* timeinfo;


		time(&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(resp.date, sizeof(resp.date), "%F %T", timeinfo);

		//strcpy(info.date, "0000-00-00 00:00:00");
		//strcpy(info.date, "");
		
		//db->saveResponse(&resp, "DISCONNECTED");
		
		if (resp.unitId > 0) {
			db->setClientStatus(resp.unitId, 0);

			DBEvent event;
			event.unitId = clients[Info.client].id;
			strftime(event.dateTime, sizeof(event.dateTime), "%F %T", timeinfo);
			event.eventId = 202;
			strcpy(event.title, "disconnected");
			strcpy(event.info, "");
			db->insertEvent(&event);
		}
		
		broadcast(&resp);
		clients.erase(Info.client);
		rClients.erase(Info.client);
		



		
	}
	
	bool Server::isSyncMsg(ConnInfo Info) {


		//double diffClock = (double(Info.clock) - double(mClock)) / CLOCKS_PER_SEC;
		//cout << "clock " << diffClock << endl;

		SyncMsg* sync_msg = (SyncMsg*)Info.buffer;

		char name[12];
		//cout << " Syncro: "<< sync_msg->Keep_Alive_Header << endl << Info.buffer << endl;
		//std::cout << "clock: "<< Info.clock  << " chrono " << (double(Info.clock-mClock) / CLOCKS_PER_SEC) << endl;
		
		clients[Info.client].clock = Info.clock;
		clients[Info.client].header = sync_msg->Keep_Alive_Header;

		for (std::map<SOCKET, GTClient>::iterator it = clients.begin(); it != clients.end(); ++it) {

			//if (it->second.type != 2) {
				printf("%10d", it->second.header);
				printf("%20s", it->second.address);

				printf("%14s", it->second.name);
				printf("%8d", it->second.id);
				printf("%10d", it->second.socket);
				printf("%10d", it->second.version_id);
				printf("%12d\n", it->second.type);
			//}
			

		}
		
		
		if (db->isVersion(sync_msg->Keep_Alive_Header)) {




			std::cout << "synchronization " << std::endl;
			//printf(ANSI_COLOR_CYAN "---> verification of sync (%lu)..(%d).\n" ANSI_COLOR_RESET, sync_msg->Keep_Alive_Device_ID, sync_msg->Keep_Alive_Header);
			//puts(sync_msg->Keep_Alive_Device_ID));

			sprintf(name, "%lu", sync_msg->Keep_Alive_Device_ID);
			//printf("\nasync %d\n", sync_msg->Keep_Alive_Device_ID);
			std::cout << "Info.client " << Info.client << "  name: " << name << "\n\n";
			if (mDevices.find(name) != mDevices.end()) {

				if (mDevices[name].socket != Info.client) {
					SOCKET oldSocket = mDevices[name].socket;
					mDevices[name].type = 0;
					mDevices[name].socket = 0;
					mDevices[name].version_id = 0;
					clients.erase(oldSocket);
					mDevices.erase(name);
					
				}
				//std::cout << "Existe name "<< name << "     - info " << mDevices[name].socket << " vs. " << Info.client << "\n\n";
			}
			
			
			mDevices[name] = clients[Info.client];


			time_t rawtime;
			struct tm* timeinfo;


			time(&rawtime);
			timeinfo = localtime(&rawtime);

			

			if (mDevices[name].type != 2) {
				//std::cout << "Validating version_id " << mDevices[name].version_id << "\n\n";
				
				
				clients[Info.client].type = 2;
				clients[Info.client].status = 1;
				

				mDevices[name].type = 2;
				strcpy(mDevices[name].device_id, (const char*)name);
				strcpy_s(mDevices[name].name, sizeof(mDevices[name].name), (const char*)name);

				InfoClient cInfo = db->getInfoClient(name);
				mDevices[name].id = cInfo.unit_id;
				mDevices[name].version_id = cInfo.version_id;

				db->setClientStatus(mDevices[name].id, 1);

				clients[Info.client].id = cInfo.unit_id;
				clients[Info.client].version_id = cInfo.version_id;
				strcpy(clients[Info.client].device_id, (const char*)name);
				strcpy_s(clients[Info.client].name, sizeof(clients[Info.client].name), (const char*)name);



				setUnitName(cInfo.unit_id, name);
				setClientName(cInfo.unit_id, cInfo.name);

				RCommand info;
				info.header = 0;
				info.commandId = 0;
				info.id = 0;
				info.index = 0;
				info.level = 0;
				strcpy(info.message, "CONNECTING");
				info.mode = 0;
				info.type = 5;
				info.typeMessage = ClientMsg::Connecting;
				strcpy(info.unit, name);
				strcpy(info.user, name);
				strcpy(info.name, cInfo.name);
				
				

				strftime(info.date, sizeof(info.date), "%F %T", timeinfo);

				//strcpy(info.date, "0000-00-00 00:00:00");
				//strcpy(info.date, "");
				info.unitId = cInfo.unit_id;

				//db->setClientStatus(info.unitId, 1, info.date);
				//db->saveResponse(&info, "CONNECTED");

				DBEvent event;
				event.unitId = info.unitId;
				strftime(event.dateTime, sizeof(event.dateTime), "%F %T", timeinfo);
				event.eventId = 201;
				strcpy(event.title, "CONNECTED");
				strcpy(event.info, "");
				db->insertEvent(&event);

				broadcast(&info);
				time(&info.time);

				cout << "Unit " << cInfo.unit_id << ", name: "<< name << " is connected " << endl;
			} else {

				db->setClientStatus(mDevices[name].id, 1);

				DBEvent event;
				event.unitId = mDevices[name].id;
				strftime(event.dateTime, sizeof(event.dateTime), "%F %T", timeinfo);
				event.eventId = 203;
				strcpy(event.title, "synch");
				strcpy(event.info, "");
				db->insertEvent(&event);
				//mDevices[name].clock = clock();
				//cout << "Algo Raro aqui!!!" << endl;
			}

			
			

			//db->saveEvent("88", 4);
			//printf(ANSI_COLOR_RED "SYNC MESSAGGE FROM: (%s) %d, version: %d \n" ANSI_COLOR_RESET, name, mDevices[name].id, mDevices[name].version_id);
			std::cout << Color::_red() << "Sync from: " << Color::_reset() << name << std::endl;
			send(Info.client, Info.buffer, Info.valread, 0);// return the sycm message

			//const char* buf = "$WP+VER=0000,?";
			//send(Info.client, buf, strlen(buf), 0);
			return true;
		}
		

		//cout << " rechazando un synch" << endl;
		return false;
	}

	IdHeader * Server::getMsgHeader(const char* msg) {
		IdHeader * header = (IdHeader*)msg;
		return header;
	}
	
	bool Server::evalMessage(ConnInfo Info, const char* message) {
		
		//printf("evalMessage: %s\n", message);

		Command* cmd = (Command*)message;

		//printf("token. %i \n", cmd->token);
		//printf("id. %s \n", cmd->id);
		//printf("message. %s \n", cmd->message);

		if (cmd->token == 9) {
			DeviceMSG * m2 = (DeviceMSG*) cmd->message;
			//m2->id[sizeof(m2->id)] = '\0';
			//printf("m2 id. %d: %s \n", sizeof(m2->id), m2->id);
			//printf("m2 id. %d: %s \n", sizeof(m2->message), m2->message);

			if (mDevices.count(m2->id)>0) {
				//printf("si es igual.\n");
				send(mDevices[m2->id].socket, m2->message, strlen(m2->message), 0);
			}
				
			for (std::map<string, GTClient>::iterator it = mDevices.begin(); it != mDevices.end(); it++) {
				if (m2->id == it->first.c_str()) {
					//printf("si es igual.\n");
				}

					
				//printf("there are a client..%s..\n", it->first.c_str() );
					
					
			}



		}


		switch (cmd->token) {
		case '*':
			puts("* asterisco");
			//shutdown(Info.client, 0);
			disconect(Info.client);
			break;
		case '$':
			//puts("$ dolar");
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
			//puts(message);
			
			break;

		}

		return false;
	}

	bool Server::deviceMessage(ConnInfo Info) {

		//printf(ANSI_COLOR_MAGENTA "esto es un device message\n");


		//std::string string = (char*)message;
		std::stringstream ss((char*)Info.buffer);
		std::string to;
		int i = 0;

		std::string result[50];
		int len;
		int nLine = 0;
		//std::cout << "My Buffer " << Info.buffer << std::endl;
		if (Info.buffer != NULL) {
			while (std::getline(ss, to)) {//, '\n'
				//std::cout << "My Command " << to.c_str() << std::endl;
				std::cout << to.c_str() << "\n\n";
				
				/*
				Tool::getTracking(result, len, to.c_str());
				if (len >= 5) {
					//cout << ANSI_COLOR_CYAN "Saving Track: mode 5" << endl;
					//db->saveTrack(clients[Info.client].device_id, result[4].c_str());
					
					if (clients[Info.client].device_id, result[4].c_str()) {
						cout << Color::_yellow() << "Saving Track from: " << Color::_reset() << getUnitName(clients[Info.client].id)  << endl;
					}
					continue;
				}
				*/
				

				Tool::getCommand(result, len, to.c_str());
				
				nLine++;


				//printf(ANSI_COLOR_CYAN "linea: %d\n", nLine);
				
				
				if (len > 0) {
					/*
					std::cout << Color::_yellow() << "Receiving From: " << Color::_reset()
						<< clients[Info.client].device_id << Color::_green() << " Message: " << Color::_yellow() << to.c_str() << std::endl;
						*/


					//cout << "es un COMANDO de " << clients[Info.client].device_id <<  endl;
					//cout << "la longitud del resultado es " << len << endl;
					CommandResult  rCommand = {
						result[2],
						result[3],
						result[4],
						result[5]

					};

					//RCommand response;
					
					RCommand unitResponse;

					unitResponse.header = 0;

					db->getIndexCommand(clients[Info.client].device_id, &rCommand, &unitResponse);

					//std::cout << " el TAG es " << rCommand.tag << "\n\n";
					if (rCommand.tag == "+2") {
						//std::cout << " el TAG es (x1)" << rCommand.tag << "\n\n";
						db->updateCommand(unitResponse.unitId, unitResponse.commandId, unitResponse.index, 2, rCommand.params);
					}
					else {
						//std::cout << " el TAG es (x2)" << rCommand.tag << "\n\n";
						db->updateCommand(unitResponse.unitId, unitResponse.commandId, unitResponse.index, 1, "");
					}
					

					//std::cout << " ---- Command Id " << unitResponse.commandId << std::endl;
					//std::cout << " ---- Index " << unitResponse.index << std::endl;

					db->infoCommand(clients[Info.client].device_id, &rCommand, &unitResponse);
					
					time_t now;
					time(&now);

					
					unitResponse.delay = difftime(now, unitResponse.time);
					std::cout << Color::_magenta() << " delay Time: " << unitResponse.delay << Color::_reset() << std::endl;


					//db->saveResponse(&unitResponse, to.c_str());
					
					//time_t rawtime;
					struct tm* timeinfo;
					time(&now);
					timeinfo = localtime(&now);

					DBEvent event;
					event.unitId = unitResponse.unitId;
					event.eventId = 210;
					strftime(event.dateTime, sizeof(event.dateTime), "%F %T", timeinfo);
					strcpy(event.title, unitResponse.command);
					strcpy(event.user, unitResponse.user);
					strcpy_s(event.info, sizeof(event.info), to.c_str());

					db->insertEvent(&event);


					db->getPending(clients[Info.client].device_id, &rCommand, &unitResponse);


					if (unitResponse.type == 2) {
						
						db->deviceConfig(clients[Info.client].device_id, &rCommand);
					}
					
					//db->evalPending(clients[Info.client].device_id, &rCommand, unitResponse.type);
					//strcpy_s(response.message, strlen(response.message)+1, to.c_str());
					strcpy(unitResponse.message, to.c_str());
					strcpy(unitResponse.name, getClientName(unitResponse.unitId).c_str());
					unitResponse.typeMessage = ClientMsg::CommandResponse;
					broadcast(&unitResponse);
				} else {
					//cout << "es un track" << endl;
					//cout << Color::_cyan() << "Saving Track" << Color::_reset()  << endl;
					//cout << ANSI_COLOR_CYAN "Saving Track: " << mClients[unit_id].device_id << endl;
					if (db->saveTrack(clients[Info.client].device_id, to.c_str())) {
						cout << Color::_cyan() << "Saving Track from: " << Color::_reset() << getUnitName(clients[Info.client].id)   << endl;
						//cout << Color::_cyan() << "--- Track: " << Color::_reset() << to.c_str() << endl;
					}
					
					webcar->insertTrack(clients[Info.client].device_id, to.c_str());
				}
				
			}
		}

		return false;
	}
	
	void Server::broadcast(RCommand * response) {
		//cout << endl << "***** *** Entrando a un Broadcast; " << rClients.size() << endl;
		
		char buffer[512];
		
		response->header = 10050;

		memcpy(buffer, &(*response), sizeof(RCommand));
		
		for (std::map<SOCKET, RClient>::iterator it = rClients.begin(); it != rClients.end(); ++it) {
			send(it->second.socket, buffer, (int)sizeof(buffer), 0);
		}
		
		memset(&buffer, 0, sizeof(buffer));//clear the buffer
		
	}
	
	std::string Server::getUnitName(int unitId) {
		return mUnitName[unitId];
	}
	
	void Server::setUnitName(int unitId, std::string name) {
		mUnitName[unitId] = name;
	}

	std::string Server::getClientName(int unitId) {
		return mClientName[unitId];
	}

	void Server::setClientName(int unitId, std::string name) {
		mClientName[unitId] = name;
	}

}