#include <stdio.h>
#include "Server.h"

using namespace rapidjson;
using namespace std;

char* GenerateCheckSum(char* buf, long bufLen)
{
	static char tmpBuf[4];
	long idx;
	unsigned int cks;

	for (idx = 0L, cks = 0; idx < bufLen; cks += (unsigned int)buf[idx++]);
	sprintf(tmpBuf, "%02X", (unsigned int)(cks % 256));
	return(tmpBuf);
}

int getCheckSum(char* buf, long bufLen) {
	static char tmpBuf[4];
	long idx;
	unsigned int cks;

	for (idx = 0L, cks = 0; idx < bufLen; cks += (unsigned int)buf[idx++]);
	//sprintf(tmpBuf, "%03d", (unsigned int)(cks % 256));
	return(cks % 256);
}

namespace GT {
	std::mutex m;
	std::mutex m2;
	std::mutex m3;
	std::mutex m4;
	
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

	void runTimeOut(std::map < SOCKET, GTClient2> * clients, Server* s, int keepAliveTime) {

		while (true) {
			
			std::this_thread::sleep_for(std::chrono::seconds(10));
			//m2.lock();
			s->isAlive();
			//m2.unlock();
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
		
		/*
		std::string w[40];
		int len;
		Tool::getCommand(w, len, "$ok:fat32+xx1=yan,esteban");
		*/

		keepAliveTime = pConfig.keep_alive;
		waitTime = pConfig.waitTime;
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
		//InfoPending infoPending;
		//db->getInfoPending(274,&infoPending);
		std::thread* threadTimeOut = new std::thread(runTimeOut, &clients, this, keepAliveTime);
		return true;
	}

	void Server::onConnect(ConnInfo Info) {

		auto client = clients.find(Info.client);

		if (client != clients.end()) {
			clients.erase(client);
		}

		std::cout << Color::_green() << "\nNew Client: Id = " << Info.client << Color::_reset() << "\n";

		
		clients.insert({
			Info.client, 
			{
				Info.client,
				0,
				-2,
				"",
				"",
				-2,
				Info.clock,
				Info.clock,
				"",
				1
			}
		});
			
		strcpy_s(clients[Info.client].name, sizeof clients[Info.client].name, "nameless");
		strcpy_s(clients[Info.client].address, sizeof clients[Info.client].address, Info.address);

	}
	
	void Server::onMessage(ConnInfo Info) {
		//printf("recibiendo -> buffer: %s\n", Info.buffer);
		
		auto x = clients.find(Info.client);

		if (x != clients.end()) {
			x->second.lastClock = clock();
		}
		
		/*
		if (clients.count(Info.client) <= 0) {
			std::cout << Color::_red() << Color::bwhite() << "\nClient Dead: Id = " << Info.client << Color::_reset() << std::endl;
			//printf("error client dead!!!!\n");
			return;
		}
		*/
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

		if (isSyncMsg2(Info)) {
			std::cout << " ES UN istartek\n\n";
		}

		IdHeader* h = getMsgHeader(Info.buffer);
		//printf("\nHeader %d, Type %d\n", h->header, h->type);
		unsigned short type = getHeader(Info);

		//printf("Info.client %d, type: %d\n", Info.client, clients[Info.client].type);

		if (clients[Info.client].type == 2) {
			//std::cout << " HOLA \n\n";
			//printf("Info.Device %s, %d, version: %d\n", clients[Info.client].device_id, clients[Info.client].id, clients[Info.client].version_id);
			
			std::string buf = Info.buffer;
			std::string name = isName("&&[^,]+,(\\d+),\\d+", buf);
			if (name != "") {
				deviceMessage2(Info);
			}
			else {
				deviceMessage(Info);
			}

			

			//return;
		}
		return;
		evalMessage(Info, Info.buffer);
	}

	Server::~Server() {

		//db->initStatus();
	}
	
	unsigned short Server::getHeader(ConnInfo Info) {
		SOCKET socket = Info.client;
		auto found = clients.find(socket);

		if (found == clients.end()) {
			return false;
		}

		auto & webclient = found->second;


		IdHeader* header = (IdHeader*)Info.buffer;
		std::string command = "";
		
		//std::cout << "Header " << header->header << std::endl;
		/* message from Websocket Server == 10001 */

		if (header->header == 10001) {
			RequestConnection* r = (RequestConnection*)Info.buffer;

			SOCKET oldSocket = getSocket(r->name);

			if (oldSocket != 0 && oldSocket != socket) {
				std::cout << "oldSocket: " << oldSocket << "  new is : " << socket << "\n\n";
				disconnect(oldSocket);
				clients.erase(oldSocket);
			}

			std::string str;

			
			webclient.type = 1;
			strcpy_s(webclient.name, sizeof webclient.name, r->name);
			strcpy_s(webclient.user, sizeof webclient.user, r->user);
			
			std::cout << "Connecting WEB SOCKET " << r->name << std::endl;

			return 0;

		}
		/*
		if (header->header == 88810001) {
			std::cout << "header == 10001 " << header->header << std::endl;
			if (rClients.find(Info.client) != rClients.end()) {
				rClients.erase(Info.client);

				std::cout << "Existe Cliente WEBSOCKET "<<  "\n\n";
			}


			std::string str;

			RequestConnection* r = (RequestConnection*)Info.buffer;
			clients[Info.client].type = 1;
			strcpy_s(clients[Info.client].name, sizeof(clients[Info.client].name), r->name);
			
			rClients[Info.client] = {};
			
			strcpy_s(rClients[Info.client].name, sizeof(rClients[Info.client].name), r->name);
			strcpy_s(rClients[Info.client].user, sizeof(rClients[Info.client].user), r->user);

			rClients[Info.client].type = 3;
			rClients[Info.client].status = 1;
			rClients[Info.client].socket = Info.client;

			cout << "Cliente Name: " << r->name << endl;
			cout << "User: " << r->user << endl;
			return 0;

		}
		*/
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

			SOCKET deviceSocket = getSocket(r->unitId);

			auto found = clients.find(deviceSocket);
			if (found == clients.end()) {


				std::string message = "SENDING TO NOTHING ";
				std::cout << "Sending message: " << str << " to Nothing\n\n";

				char buffer[1024];
				strcpy_s(response.message, sizeof response.message, message.c_str());
				
				str = r->unit;
				strcpy_s(response.unit, sizeof response.unit, r->unit);

				str = r->user;
				strcpy_s(response.user, sizeof response.user, str.c_str());

				memcpy(buffer, &response, sizeof(response));
				send(socket, buffer, (int)sizeof(buffer), 0);
				return 0;
			}

			auto & device = found->second;

			char buffer[1024];
			strcpy_s(response.message, sizeof response.message, str.c_str());

			str = r->unit;
			strcpy_s(response.unit, sizeof response.unit, r->unit);

			str = r->user;
			strcpy_s(response.user, sizeof response.user, str.c_str());
			


			memcpy(buffer, &response, sizeof(response));
			
			send(socket, buffer, (int)sizeof(buffer), 0);
			send(deviceSocket, r->message, strlen(r->message), 0);
			
			std::cout 
				<< Color::_green() << "Sending: " << response.message 
				<<  Color::_reset() << " to: "
				<< device.name << " (" << r->unit << ")\n";



			


			return 0;

		}
		if (header->header == 10300) {
			RCommand* r = (RCommand*)Info.buffer;

			InfoPending infoPending;
			db->getInfoPending(r->id, &infoPending);

			std::cout << "Pending Id " << infoPending.command << std::endl;

			SOCKET deviceSocket = getSocket(r->unitId);

			auto found = clients.find(deviceSocket);
			if (found != clients.end()) {
				send(deviceSocket, infoPending.command.c_str(), strlen(infoPending.command.c_str()), 0);
			}
			
		}
		if (header->header == 10100) {
			RCommand* r = (RCommand*)Info.buffer;

			SOCKET deviceSocket = getSocket(r->unitId);

			auto found = clients.find(deviceSocket);
			if (found == clients.end()) {
				return 0;
			}

			auto & device = found->second;

			

			RCommand response;
			response.header = 10021;
			response.mode = r->mode;
			response.type = r->type;
			response.id = r->id;
			response.unitId = r->unitId;
			response.level = r->level;
			std::string str = r->message;
			char buffer[1024];
			strcpy_s(response.message, sizeof response.message, str.c_str());
			//strcpy(response.message, str.c_str());

			str = r->unit;


			strcpy_s(response.unit, sizeof response.unit, r->unit);
			//strcpy_s(response.unit, str.c_str());

			str = r->user;
			strcpy_s(response.user, sizeof response.user, str.c_str());
			//strcpy_s(response.user, str.c_str());


			memcpy(buffer, &response, sizeof(response));
			send(socket, buffer, (int)sizeof(buffer), 0);


			//send(mDevices[getUnitName(r->unitId)].socket, r->message, strlen(r->message), 0);

			std::cout << Color::_green() << "Sending: " << response.message
				<< Color::_reset() << " to: "
				<< device.name << " (" << r->unit << ")\n";
			/*
			std::cout << "Header: " << response.header << std::endl;
			std::cout << "Message: " << response.message << std::endl;
			std::cout << "UnitId: " << response.unitId << std::endl;
			std::cout << "Mode: " << response.mode << std::endl;
			std::cout << "User: " << response.user << std::endl;
			std::cout << "Unit: " << response.unit << std::endl;
			*/

			int value = disconnect(device.socket);

			if (value > 0) {
				std::cout << "DISCONECTING TO: " << device.socket << "\n";
				
				closeClient(device.socket);
				
			}
			return 0;

		}


		


		return header->type;
	}
	
	void Server::closeClient(SOCKET client) {

		disconnect(client);
		
		deleteClient(client);
		
	}
	void Server::deleteClient(SOCKET client) {

		std::cout << "ON CLOSE ID: " << client << " \n";
		
		clients[client].name;
		RCommand resp;
		resp.unitId = clients[client].id;
		resp.header = 0;
		resp.commandId = 0;
		resp.id = 0;
		resp.index = 0;
		resp.level = 0;

		resp.mode = 0;
		resp.type = 6;
		resp.typeMessage = ClientMsg::Disconnecting;
		strcpy_s(resp.message, sizeof resp.message, "DISCONNECTED");
		strcpy_s(resp.unit, sizeof resp.unit, clients[client].name);
		strcpy_s(resp.user, sizeof resp.user, clients[client].name);
		strcpy_s(resp.name, sizeof resp.name, getClientName(clients[client].id).c_str());

		time_t rawtime;
		struct tm* timeinfo;


		time(&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(resp.date, sizeof(resp.date), "%F %T", timeinfo);

		//strcpy(info.date, "0000-00-00 00:00:00");
		//strcpy(info.date, "");

		//db->saveResponse(&resp, "DISCONNECTED");

		if (resp.unitId > 0) {
			setClientStatus(resp.unitId, 0);

			DBEvent event;
			event.unitId = clients[client].id;
			strftime(event.dateTime, sizeof(event.dateTime), "%F %T", timeinfo);
			event.eventId = 202;
			strcpy_s(event.title, sizeof event.title, "disconnected");
			strcpy_s(event.info, sizeof event.info, "");
			insertEvent(&event);
		}
		m.lock();
		clients.erase(client);
		broadcast(&resp);
		m.unlock();
		//std::cout << "closing Client: " << client << " size:" << clients.size() << " \n";
		
		//rClients.erase(client);
		//std::cout << "closing Client: " << client << " size:" << clients.size() << " \n";
		
	}
	void Server::onClose(ConnInfo Info) {
		
		
		deleteClient(Info.client);
		

	}
	
	bool Server::isSyncMsg(ConnInfo Info) {
		SOCKET socket = Info.client;
		auto found = clients.find(socket);

		if (found == clients.end()) {
			return false;
		}

		

		
		

		SyncMsg* sync_msg = (SyncMsg*)Info.buffer;

		
		//cout << " Syncro: "<< sync_msg->Keep_Alive_Header << endl << Info.buffer << endl;
		//printf("NEW ID?? %lu socket(%d) H(%d)\n", sync_msg->Keep_Alive_Device_ID, Info.client, sync_msg->Keep_Alive_Header);
		//std::cout << "clock: "<< Info.clock  << " chrono " << (double(Info.clock-mClock) / CLOCKS_PER_SEC) << endl;
		
		//clients[Info.client].clock = Info.clock;
		//clients[Info.client].header = sync_msg->Keep_Alive_Header;
		
		
		
		if (isVersion(sync_msg->Keep_Alive_Header)) {
			auto& client = found->second;
			double timeInSeconds = 0;
			clock_t endTime = clock();
			char name[12];
			
			//printf(ANSI_COLOR_CYAN "---> verification of sync (%lu)..(%d).\n" ANSI_COLOR_RESET, sync_msg->Keep_Alive_Device_ID, sync_msg->Keep_Alive_Header);
			//puts(sync_msg->Keep_Alive_Device_ID));

			sprintf(name, "%lu", sync_msg->Keep_Alive_Device_ID);
			//std::cout << "synchronization Name " << name << " SOCKET " << socket << std::endl;
			//printf("\nasync %d\n", sync_msg->Keep_Alive_Device_ID);
			//std::cout << "socket: " << socket << "  name: " << name << "\n\n";
			SOCKET oldSocket = getSocket(name);

			if (oldSocket != 0 && oldSocket != socket) {
				std::cout << "oldSocket: " << oldSocket << "  new is : " << socket << "\n\n";
				disconnect(oldSocket);
				clients.erase(oldSocket);
			}

			time_t rawtime;
			struct tm* timeinfo;

			time(&rawtime);
			timeinfo = localtime(&rawtime);
			
			
			if (client.type != 2) {
				
				client.type = 2;
				//clients[socket].type = 2;
				client.status = 1;
				strcpy_s(client.name, sizeof client.name, (const char*)name);

				InfoClient cInfo = getInfoClient(name);
				client.id = cInfo.unit_id;
				client.formatId = cInfo.format_id;

				//mDevices[name].version_id = cInfo.version_id;

				setClientStatus(client.id, 1);


				setUnitName(client.id, name);
				setClientName(client.id, cInfo.name);

				RCommand info;
				info.header = 0;
				info.commandId = 0;
				info.id = 0;
				info.index = 0;
				info.level = 0;
				strcpy_s(info.message, sizeof info.message, "CONNECTING");
				info.mode = 0;
				info.type = 5;
				info.typeMessage = ClientMsg::Connecting;
				strcpy_s(info.unit, sizeof info.unit, name);
				strcpy_s(info.user, sizeof info.user, name);
				strcpy_s(info.name, sizeof info.name, cInfo.name);

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
				strcpy_s(event.title, sizeof event.title, "CONNECTED");
				strcpy_s(event.info, sizeof event.info, "");
				insertEvent(&event);

				broadcast(&info);
				time(&info.time);

				cout << "Unit " << client.id << ", name: "<< name << " is connected " << endl;
			} else {

				setClientStatus(client.id, 1);

				DBEvent event;
				event.unitId = client.id;
				strftime(event.dateTime, sizeof(event.dateTime), "%F %T", timeinfo);
				event.eventId = 203;
				strcpy_s(event.title, sizeof event.title, "synch");
				strcpy_s(event.info, sizeof event.info, "");
				insertEvent(&event);
				//mDevices[name].clock = clock();
				//cout << "Algo Raro aqui!!!" << endl;
			}

			
			
			

			//db->saveEvent("88", 4);
			//printf(ANSI_COLOR_RED "SYNC MESSAGGE FROM: (%s) %d, version: %d \n" ANSI_COLOR_RESET, name, mDevices[name].id, mDevices[name].version_id);
			std::cout << Color::_cyan() << "Synchronization from: " << Color::_reset() << name << std::endl;
			
			// return the sycm message
			send(socket, Info.buffer, Info.valread, 0);

			//const char* buf = "$WP+VER=0000,?";
			//send(Info.client, buf, strlen(buf), 0);
			return true;
		}
		

		//cout << " rechazando un synch" << endl;
		return false;
	}

	bool Server::isSyncMsg2(ConnInfo Info) {
		SOCKET socket = Info.client;
		auto found = clients.find(socket);

		if (found == clients.end()) {
			return false;
		}

		//std::map<string> li = IStartek.getResponse(Info.buffer);

		

		

		//std::cout << " EL BUFFER ES.. " << Info.buffer << "\n";


		iStartekHeader* sync_msg = (iStartekHeader*)Info.buffer;

		std::string header = "";

		header += sync_msg->token[0];
		header += sync_msg->token[1];
		//sync_msg->token[2] = '\0';

		

		//cout << " Syncro: "<< sync_msg->Keep_Alive_Header << endl << Info.buffer << endl;
		//printf("NEW ID?? %lu socket(%d) H(%d)\n", sync_msg->Keep_Alive_Device_ID, Info.client, sync_msg->Keep_Alive_Header);
		//std::cout << "clock: "<< Info.clock  << " chrono " << (double(Info.clock-mClock) / CLOCKS_PER_SEC) << endl;

		//clients[Info.client].clock = Info.clock;
		//clients[Info.client].header = sync_msg->Keep_Alive_Header;
		//std::cout << " Header 1 " << header << "\n";
		
		

		if (header == "&&") {

			auto map = IStartek.getResponse(Info.buffer);

			//std::cout << " ID " << map["ID"].c_str() << "\n";
			auto& client = found->second;
			double timeInSeconds = 0;
			clock_t endTime = clock();
			char name[12];

			//printf(ANSI_COLOR_CYAN "---> verification of sync (%lu)..(%d).\n" ANSI_COLOR_RESET, sync_msg->Keep_Alive_Device_ID, sync_msg->Keep_Alive_Header);
			//puts(sync_msg->Keep_Alive_Device_ID));

			//sprintf(name, "%lu", "2012000413");
			sprintf(name, "%s", map["ID"].c_str());
			//std::cout << "synchronization Name " << name << " SOCKET " << socket << std::endl;
			//printf("\nasync %d\n", sync_msg->Keep_Alive_Device_ID);
			//std::cout << "socket: " << socket << "  name: " << name << "\n\n";
			SOCKET oldSocket = getSocket(name);

			if (oldSocket != 0 && oldSocket != socket) {
				std::cout << "oldSocket: " << oldSocket << "  new is : " << socket << "\n\n";
				disconnect(oldSocket);
				clients.erase(oldSocket);
			}

			time_t rawtime;
			struct tm* timeinfo;

			time(&rawtime);
			timeinfo = localtime(&rawtime);


			if (client.type != 2) {
				//puts("yanny......... type != 2\n\n");
				client.type = 2;
				//clients[socket].type = 2;
				client.status = 1;
				strcpy_s(client.name, sizeof client.name, (const char*)name);

				InfoClient cInfo = getInfoClient(name);
				client.id = cInfo.unit_id;
				client.formatId = cInfo.format_id;

				//mDevices[name].version_id = cInfo.version_id;

				setClientStatus(client.id, 1);


				setUnitName(client.id, name);
				setClientName(client.id, cInfo.name);

				RCommand info;
				info.header = 0;
				info.commandId = 0;
				info.id = 0;
				info.index = 0;
				info.level = 0;
				strcpy_s(info.message, sizeof info.message, "CONNECTING");
				info.mode = 0;
				info.type = 5;
				info.typeMessage = ClientMsg::Connecting;
				strcpy_s(info.unit, sizeof info.unit, name);
				strcpy_s(info.user, sizeof info.user, name);
				strcpy_s(info.name, sizeof info.name, cInfo.name);

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
				strcpy_s(event.title, sizeof event.title,"CONNECTED");
				strcpy_s(event.info, sizeof event.info, "");
				insertEvent(&event);

				broadcast(&info);
				time(&info.time);

				cout << "Unit " << client.id << ", name: " << name << " is connected " << endl;
			}
			else {

				setClientStatus(client.id, 1);

				DBEvent event;
				event.unitId = client.id;
				strftime(event.dateTime, sizeof(event.dateTime), "%F %T", timeinfo);
				event.eventId = 203;
				strcpy_s(event.title, sizeof event.title, "synch");
				strcpy_s(event.info, sizeof event.info, "");
				//insertEvent(&event);
				//mDevices[name].clock = clock();
				//cout << "Algo Raro aqui!!!" << endl;
			}





			//db->saveEvent("88", 4);
			//printf(ANSI_COLOR_RED "SYNC MESSAGGE FROM: (%s) %d, version: %d \n" ANSI_COLOR_RESET, name, mDevices[name].id, mDevices[name].version_id);
			std::cout << Color::_cyan() << "Synchronization from: " << Color::_reset() << name << std::endl;

			// return the sycm message
			send(socket, Info.buffer, Info.valread, 0);

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

		

		switch (cmd->token) {
		case '*':
			puts("* asterisco");
			//shutdown(Info.client, 0);
			disconnect(Info.client);
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
		
		static int band = 0;
		if (band > 5) {
			return false;
		}
		band++;
		char buf[200];

		

		send(Info.client, Info.buffer, Info.valread, 0);
		static char packNo = 58+10;
		int packLength = 0;
		std::string id = "861157040200913";
		std::string commandCode = "110";
		std::string commandData = "2024000100";
		int checkSum = 0;
		
		std::string cmd1 = "," + id + "," + commandCode + "," + commandData;
		packLength = cmd1.size();

		std::string command = "$$" + std::string (1, packNo %256) + std::to_string(packLength) + cmd1;

		printf("command: %s\n", command.c_str());
			//std::to_string(a++)+;
		packNo++;

		if (packNo == 126) {
			packNo = 58;
		}
		char tmpBuf[4];
		char buffer[1024];


		printf("%p", buffer);

		memcpy(buffer, command.c_str(), command.size());
		int check = (unsigned int)getCheckSum((char*)command.c_str(), command.size());
		
		sprintf(tmpBuf, "%02X\r\n", check);
		

		memcpy(buffer+ command.size(), tmpBuf, 4);
		memcpy(buffer + command.size()+4, "\0", 1);

		//std::string x = std::string(buffer);

		
		
		

		send(Info.client, buffer, command.size() + 4, 0);

		//send(Info.client, "yanny\0", 6, 0);
		std::cout << "END COMMAND " << buffer << "\n\n";
		return false;
	}

	bool Server::deviceMessage(ConnInfo Info) {

		

		SOCKET socket = Info.client;
		auto found = clients.find(socket);

		if (found == clients.end()) {
			return false;
		}

		
		auto& client = found->second;

		//printf(ANSI_COLOR_MAGENTA "esto es un device message\n");


		//std::string string = (char*)message;
		std::stringstream ss((char*)Info.buffer);


		//std::string s2(Tool::alphaNumeric(Info.buffer));


		std::string to;
		int i = 0;

		std::string result[50];
		int len;
		int nLine = 0;
		//printf("puntero %p\n", Info.buffer);
		//std::cout << "My Buffer " << Info.buffer << " \n\nEND BUFFER" << std::endl;
		//std::cout << "My String " << s2 << " \n\nEND BUFFER" << std::endl;
		if (Info.buffer != NULL) {
			while (std::getline(ss, to)) {//, '\n'
				//to = "$ok:SETEVT=100,0,zzzxx,,,0,1,,,,,,,,0,0,,,,,,";
				//std::cout << "My Command " << to.c_str() << std::endl;
				//std::cout << to.c_str() << "\n\n";
				
				//std::cout << " tracking " << to.c_str() << "\n\n";

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

					getIndexCommand(client.name, &rCommand, &unitResponse);
					//std::cout << " client.name: " << client.name << "\n\n";
					//std::cout << " el TAG es " << rCommand.tag << "\n\n";
					if (rCommand.tag == "+2") {
						//std::cout << " el TAG es (x1)" << rCommand.tag << "\n\n";
						unitResponse.index = updateCommand(unitResponse.unitId, unitResponse.commandId, unitResponse.index, 2, rCommand.params);
					}
					else {
						//std::cout << " el TAG es (x2)" << rCommand.tag << "\n\n";
						unitResponse.index = updateCommand(unitResponse.unitId, unitResponse.commandId, unitResponse.index, 1, rCommand.params);
					}
					

					//std::cout << " ---- Command Id " << unitResponse.commandId << std::endl;
					//std::cout << " ---- Index " << unitResponse.index << std::endl;

					infoCommand(client.name, &rCommand, &unitResponse);
					
					time_t now;
					time(&now);

					
					unitResponse.delay = difftime(now, unitResponse.time);
					//std::cout << Color::_magenta() << " delay Time: " << unitResponse.delay << Color::_reset() << std::endl;


					//db->saveResponse(&unitResponse, to.c_str());
					
					//time_t rawtime;
					struct tm* timeinfo;
					time(&now);
					timeinfo = localtime(&now);

					DBEvent event;
					event.unitId = unitResponse.unitId;
					event.eventId = 210;
					strftime(event.dateTime, sizeof event.dateTime, "%F %T", timeinfo);
					strcpy_s(event.title, sizeof event.title, unitResponse.command);
					strcpy_s(event.user, sizeof event.user, unitResponse.user);
					strcpy_s(event.info, sizeof event.info, to.c_str());

					insertEvent(&event);


					//db->getPending(client.name, &rCommand, &unitResponse);


					if (unitResponse.type == 2) {
						
						db->deviceConfig(client.name, &rCommand);
					}
					
					//db->evalPending(clients[Info.client].device_id, &rCommand, unitResponse.type);
					//strcpy_s(response.message, strlen(response.message)+1, to.c_str());
					strcpy_s(unitResponse.message, sizeof unitResponse.message, to.c_str());
					strcpy_s(unitResponse.name, sizeof unitResponse.name, getClientName(unitResponse.unitId).c_str());
					unitResponse.typeMessage = ClientMsg::CommandResponse;
					broadcast(&unitResponse);
				} else {
					//cout << "es un track" << endl;
					//cout << Color::_cyan() << "Saving Track" << Color::_reset()  << endl;
					//cout << ANSI_COLOR_CYAN "Saving Track: " << mClients[unit_id].device_id << endl;
					std::string track(to.c_str());
					insertTrack(clients[Info.client].name, to.c_str());

					if (saveTrack(client.id, client.formatId, track.c_str())) {
						cout << Color::_yellow() << "Saving Track from: " << Color::_reset() << getUnitName(clients[Info.client].id)   << endl;

						//std::cout << "ERROR REVISAR WC MY Tracking " << to.c_str() << "\n\n";
						
						//cout << Color::_cyan() << "--- Track: " << Color::_reset() << to.c_str() << endl;
					}
					

					
				}
				
			}
		}

		return false;
	}
	

	bool Server::deviceMessage2(ConnInfo Info) {

		SOCKET socket = Info.client;
		auto found = clients.find(socket);

		if (found == clients.end()) {
			return false;
		}

		auto& client = found->second;

		//printf(ANSI_COLOR_MAGENTA "esto es un device message\n");

		//std::string string = (char*)message;
		std::stringstream ss((char*)Info.buffer);

		//std::string s2(Tool::alphaNumeric(Info.buffer));

		std::string to;
		int i = 0;

		std::string result[50];
		int len;
		int nLine = 0;
		//printf("puntero %p\n", Info.buffer);
		//std::cout << "My Buffer " << Info.buffer << " \n\nEND BUFFER" << std::endl;
		//std::cout << "My String " << s2 << " \n\nEND BUFFER" << std::endl;

		std::string message = "";

		if (Info.buffer != NULL) {
			while (std::getline(ss, to)) {//, '\n'
				message = to.substr(0, to.size() - 3);
				
				std::map<std::string, std::string> map = IStartek.getResponse(message.c_str());
				
				
				//Tool::getCommand(result, len, message.c_str());

				nLine++;


				printf(ANSI_COLOR_CYAN "linea: %d\n", nLine);
				

				if (map["cmd"] != "000" && map["cmd"] != "010" && map["cmd"] != "020") {

					std::cout << Color::_yellow() << "map[data]: " << map["data"] << Color::_reset() << "\n";

					/*
					std::cout << Color::_yellow() << "Receiving From: " << Color::_reset()
						<< clients[Info.client].device_id << Color::_green() << " Message: " << Color::_yellow() << to.c_str() << std::endl;
						*/


						//cout << "es un COMANDO de " << clients[Info.client].device_id <<  endl;
						//cout << "la longitud del resultado es " << len << endl;
					CommandResult  rCommand = {
						"OK",
						map["cmd"],
						"",
						map["data"]

					};

					//RCommand response;

					RCommand unitResponse;

					unitResponse.header = 0;

					getIndexCommand(client.name, &rCommand, &unitResponse);
					std::cout << " client.name: " << client.name << "\n\n";
					//std::cout << " el TAG es " << rCommand.tag << "\n\n";

					if (rCommand.tag == "+2") {
						//std::cout << " el TAG es (x1)" << rCommand.tag << "\n\n";
						unitResponse.index = updateCommand(unitResponse.unitId, unitResponse.commandId, unitResponse.index, 2, rCommand.params);
					}
					else {
						//std::cout << " el TAG es (x2)" << rCommand.tag << "\n\n";
						unitResponse.index = updateCommand(unitResponse.unitId, unitResponse.commandId, unitResponse.index, unitResponse.mode, rCommand.params);
					}


					//std::cout << " ---- Command Id " << unitResponse.commandId << std::endl;
					//std::cout << " ---- Index " << unitResponse.index << std::endl;

					infoCommand(client.name, &rCommand, &unitResponse);

					time_t now;
					time(&now);


					unitResponse.delay = difftime(now, unitResponse.time);
					//std::cout << Color::_magenta() << " delay Time: " << unitResponse.delay << Color::_reset() << std::endl;


					//db->saveResponse(&unitResponse, to.c_str());

					//time_t rawtime;
					struct tm* timeinfo;
					time(&now);
					timeinfo = localtime(&now);

					DBEvent event;
					event.unitId = unitResponse.unitId;
					event.eventId = 210;
					strftime(event.dateTime, sizeof event.dateTime, "%F %T", timeinfo);
					strcpy_s(event.title, sizeof event.title, unitResponse.command);
					strcpy_s(event.user, sizeof event.user, unitResponse.user);
					strcpy_s(event.info, sizeof event.info, to.c_str());

					insertEvent(&event);


					//db->getPending(client.name, &rCommand, &unitResponse);


					if (unitResponse.type == 2) {

						db->deviceConfig(client.name, &rCommand);
					}

					//db->evalPending(clients[Info.client].device_id, &rCommand, unitResponse.type);
					//strcpy_s(response.message, strlen(response.message)+1, to.c_str());
					strcpy_s(unitResponse.message, sizeof unitResponse.message,to.c_str());
					strcpy_s(unitResponse.name, sizeof unitResponse.name, getClientName(unitResponse.unitId).c_str());
					unitResponse.typeMessage = ClientMsg::CommandResponse;
					broadcast(&unitResponse);
				}
				else {
					//cout << "es un track" << endl;
					//cout << Color::_cyan() << "Saving Track" << Color::_reset()  << endl;
					//cout << ANSI_COLOR_CYAN "Saving Track: " << mClients[unit_id].device_id << endl;
					std::string tracking = IStartek.getTracking(message.c_str());

					std::string track(tracking.c_str());
					
					insertTrack(clients[Info.client].name, tracking.c_str());

					if (saveTrack(client.id, client.formatId, track.c_str())) {
						cout << Color::_yellow() << "Saving Track from: " << Color::_reset() << getUnitName(clients[Info.client].id) << endl;

						//std::cout << "ERROR REVISAR WC MY Tracking " << to.c_str() << "\n\n";

						//cout << Color::_cyan() << "--- Track: " << Color::_reset() << to.c_str() << endl;
					}



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
		
		for (std::map<SOCKET, GTClient2>::iterator it = clients.begin(); it != clients.end(); ++it) {
			if (it->second.type == 1) {
				send(it->second.socket, buffer, (int)sizeof(buffer), 0);
			}
			
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

	SOCKET Server::getSocket(int id) {
		
		for (std::map<SOCKET, GTClient2>::iterator it = clients.begin(); it != clients.end(); ++it) {
			
			if (it->second.id == id) {
				return it->first;
			}
		}
		return 0;
	}
	bool Server::saveTrack(int unitId, int formatId, const char* buffer) {
		m2.lock();
		bool result = db->saveTrack(unitId, formatId, buffer);
		m2.unlock();

		return result;
		
	}
	bool Server::isVersion(int value)
	{
		m2.lock();
		bool result = db->isVersion(value);
		m2.unlock();
		return result;
	}
	InfoClient Server::getInfoClient(string name)
	{
		m2.lock();
		InfoClient cInfo = db->getInfoClient(name);
		m2.unlock();
		return cInfo;
	}
	int Server::updateCommand(int unitId, int commandId, int index, int mode, std::string params)
	{
		m2.lock();
		int result = db->updateCommand(unitId, commandId, index, mode, params);
		m2.unlock();

		return result;
	}
	void Server::getIndexCommand(const char* unit_id, CommandResult* commandResult, RCommand* info)
	{
		m2.lock();
		db->getIndexCommand(unit_id, commandResult, info);
		m2.unlock();
	}
	void Server::infoCommand(const char* unit_id, CommandResult* commandResult, RCommand* info)
	{
		m2.lock();
		db->infoCommand(unit_id, commandResult, info);
		m2.unlock();
	}
	void Server::insertTrack(std::string name, std::string track)
	{
		m2.lock();
		webcar->insertTrack(name, track.c_str());
		m2.unlock();
		return;
	}
	bool Server::insertEvent(DBEvent* infoEvent)
	{
		m2.lock();
		db->insertEvent(infoEvent);
		m2.unlock();
		return true;
	}

	bool Server::setClientStatus(unsigned int unitId, unsigned int status)
	{
		m2.lock();
		db->setClientStatus(unitId, status);
		m2.unlock();
		return true;
	}

	SOCKET Server::getSocket(std::string name) {
		
		for (std::map<SOCKET, GTClient2>::iterator it = clients.begin(); it != clients.end(); ++it) {
			//std::cout << "1. is same " << name << " vs " << it->second.name << "\n";
	
			if (name == it->second.name) {
				//std::cout << "2.same\n";
				return it->first;
			}
		}
		return 0;
	}

	void Server::isAlive() {
		//m2.lock();
		double timeInSeconds = 0;
		double delta = 0;
		clock_t endTime = clock();
		printf("\n/**********Clients List **********/\n");
		int n = 0;
		int i = 0;
		
		std::map<SOCKET, GTClient2> cli = clients;
		for (std::map<SOCKET, GTClient2>::iterator it = cli.begin(); it != cli.end(); ++it) {
			
			if (it->second.type == 2) {
				i++;
				n = i;
			}
			
			timeInSeconds = (double(endTime - it->second.clock) / CLOCKS_PER_SEC);
			delta = (double(endTime - it->second.lastClock) / CLOCKS_PER_SEC);
			 
			printf(ANSI_COLOR_RESET);

			if (it->second.type == 0) {
				printf(ANSI_COLOR_YELLOW);
				n = 0;
			}
			
			if (it->second.type == 1) {
				printf(ANSI_COLOR_CYAN);
				n = 0;
			}
			
			printf("%3d", n);
			printf("%6d", it->second.id);
			printf("%18s", it->second.address);
			printf("%12s", it->second.name);
			//printf("%6d", it->second.header);
			printf("%12.3f", timeInSeconds);
			printf("%12.3f", delta);
			printf("%8d", int(it->second.socket));
			printf("%8d", it->second.formatId);
			printf("%6d\n", it->second.type);
			
			if (it->second.type == 0 && timeInSeconds > (double)waitTime) {
				printf("%50s\n", "-- DISCONECTING TO UNKNOWN && waitTime: %d", waitTime);
				//printf("%10d", it->second.header);
				printf("%3d", n);
				printf("%6d", it->second.id);
				printf("%18s", it->second.address);
				printf("%12s", it->second.name);
				//printf("%6d", it->second.header);
				printf("%12.3f", timeInSeconds);
				printf("%12.3f", delta);
				printf("%8d", int(it->second.socket));
				printf("%8d", it->second.formatId);
				printf("%6d\n", it->second.type);
				//disconect(it->first);
				//clients.erase(it->first);
				closeClient(it->first);
				
			}

			if (it->second.type == 2 && delta > (double)keepAliveTime) {
				printf("%50s\n", "-- DISCONECTING TO DEVICE");
				printf("%3d", n);
				printf("%6d", it->second.id);
				//printf("%10d", it->second.header);
				printf("%18s", it->second.address);
				printf("%12s", it->second.name);
				//printf("%6d", it->second.header);
				printf("%12.3f", timeInSeconds);
				printf("%12.3f", delta);
				printf("%8d", int(it->second.socket));
				printf("%8d", it->second.formatId);
				printf("%6d\n", it->second.type);
				//disconect(it->first);
				//clients.erase(it->first);
				closeClient(it->first);
				//closeClient(it->first);
			}

			printf(ANSI_COLOR_RESET);

		}
		
		printf("/*********************************/\n");
		//listSocket();
		//m2.unlock();
	}

}