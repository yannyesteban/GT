#include <stdio.h>
#include "Server.h"

using namespace rapidjson;
using namespace std;


namespace GT {
	
	std::mutex m;
	std::mutex m2;
	std::mutex m3;
	std::mutex m4;
	
	

	void runTimeOut(std::map < SOCKET, GTClient1> * clients, Server* s, int keepAliveTime) {

		while (true) {
			
			std::this_thread::sleep_for(std::chrono::seconds(10));
			
			s->isAlive();
			
		}

	}

	
	Server::~Server() {

		//db->initStatus();
	}
	
	Server::Server(SocketInfo pInfo):Socket(pInfo),debug(false), db(nullptr) {
		mClock = clock();
	}

	bool Server::init(AppConfig pConfig) {

		keepAliveTime = pConfig.keep_alive;
		
		waitTime = pConfig.waitTime;
		
		pConfig.db.debug = pConfig.debug;
				
		db = new ProtoDB(pConfig.db); 
		
		db->connect();

		synchValue = db->loadSynchValue();

		mProto = db->loadProtocols();

		identExp = db->loadIdentExp();
				
		webcar = new WC::Webcar("webcar.json");
				
		std::thread* threadTimeOut = new std::thread(runTimeOut, &clients, this, keepAliveTime);
		
		return true;
	}

	void Server::onConnect(ConnInfo Info) {

		auto client = clients.find(Info.client);

		if (client != clients.end()) {
			clients.erase(client);
		}

		std::cout << Color::_green() << "\nNew Client: Id = " << Info.client << Color::_reset() << "\n";

		clients[Info.client] = 
			{
				Info.client,
				0,
				-2,
				"nameless",
				Info.address,
				-2,
				Info.clock,
				Info.clock,
				"",
				1
			};
			
	}
	
	void Server::onMessage(ConnInfo Info) {
		
		auto client = clients.find(Info.client);

		if (client != clients.end()) {
			client->second.lastClock = clock();
		}
		
		int type = client->second.type;

		

		std::string name = isSynch(Info);

		if (name != "") {
			std::cout << "Return SYNCH Message from: " << name << "\n";

			if (type == 0) {
				defineClient(Info, name, 2, name);
			}

			if (client->second.id > 0) {
				InfoEvent event;
				event.unitId = client->second.id;
				event.eventId = 203;
				event.mode = 0;
				event.info = "";
				event.title = "SYNCH";

				insertEvent(event);
			}

			return;
		}
		
		name = isDevice(Info);

		if (name != "") {
			std::cout << "Identify Device: " << name << "\n";


			if (type == 0) {
				defineClient(Info, name, 2, name);
			}
		}

		if (isTracking(Info)) {
			return;
		}

		if (isCommand(Info)) {
			return;
		}

		evalRequest(Info);

		
		return;
	}

	void Server::defineClient(ConnInfo info, std::string name, int type, std::string user) {
		

		SOCKET oldSocket = getSocket(name);

		if (oldSocket != 0 && oldSocket != info.client) {
			std::cout << "oldSocket: " << oldSocket << "  new is : " << info.client << "\n\n";
			disconnect(oldSocket);
			clients.erase(oldSocket);
		}
		
		clients[info.client].type = type;
		clients[info.client].name = name;
		clients[info.client].user = user;

		if (type == 1) {

		}

		if (type == 2) {
			InfoClient cInfo = getInfoClient(name);
			clients[info.client].id = cInfo.unit_id;
			clients[info.client].verionId = cInfo.format_id;

			InfoEvent event;
			event.unitId = cInfo.unit_id;
			event.eventId = 201;
			event.mode = 0;
			event.info = "";
			event.title = "CONNECTED";
			
			insertEvent(event);
		}
	}

	bool Server::isTracking(ConnInfo Info){
		std::string message = Info.buffer;


		auto client = clients[Info.client];

		auto tracking = mProto[client.verionId].tracking;
		auto proto = mProto[client.verionId];		
			
			
		//std::cout << "new message " << message << "\n\n";
		//std::cout << "new proto.tracking " << proto.tracking << "\n\n";

		if (!std::regex_match(message, std::regex(proto.tracking))) {
			//std::cout << "\n\n\nerror in tracking\n\n\n";
			return false;
		}
	   	
		
		//std::cout << "new tracking " << mProto[client.verionId].tracking;
		m2.lock();
		auto data = db->decodeTracking(message, proto);
		db->evalData(data, proto);
		db->saveTrack(client.id, data);
		m2.unlock();
		wcTracking(client.id, data);

		return true;
	}
	bool Server::isCommand(ConnInfo Info){
		std::string message = Info.buffer;
		//message = "$ST+STARTLOG=0000,5,2,3,4,5";

		auto client = clients[Info.client];

		auto tracking = mProto[client.verionId].tracking;
		auto proto = mProto[client.verionId];


		//std::cout << "new message " << message << "\n\n";
		//std::cout << "new proto.decCmd " << proto.decCmd << "\n\n";

		if (!std::regex_match(message, std::regex(proto.decCmd))) {
			//std::cout << "\n\n\nerror , not command message!\n\n\n";
			return false;
		}

		auto res = cmd.encode(proto.decCmd, proto.decCmdNames, message);

		int index = cmd.toInteger(res["index"]);
		int unitId = client.id;
		std::string command = res["command"];
		std::string params = res["params"];

		

		std::vector<std::string> list = cmd.splitParams(params, proto.paramSep);
		m2.lock();
		auto strList = db->encodeToArray(list);
		
		db->updateCommand(unitId, command, index, strList);
		m2.unlock();
		/*
		std::cout << unitId << "\n";
		std::cout << command << "\n";
		std::cout << index << "\n";
		std::cout << params << "\n";
		
		*/
		                                                                             

		return true;
	}
	
	std::string Server::isSynch(ConnInfo Info){

		std::string id = "";

		SyncMsg* sync_msg = (SyncMsg*)Info.buffer;

		for (auto& header : synchValue) {
			if (header == sync_msg->Keep_Alive_Header) {
				id = std::to_string(sync_msg->Keep_Alive_Device_ID);
				send(Info.client, Info.buffer, Info.valread, 0);
				break;
			}
		}

		return id;
	}

	std::string Server::isDevice(ConnInfo Info) {

		std::string message = Info.buffer;
		std::string id = "";
		
		for (auto& exp : identExp) {
			id = cmd.getName(exp, message);
			if (id != "") {
				break;
			}
		}

		return id;
	}

	unsigned short Server::evalRequest(ConnInfo Info)
	{
		
		SOCKET socket = Info.client;
		auto found = clients.find(socket);

		if (found == clients.end()) {
			return false;
		}

		IdHeader* header = (IdHeader*)Info.buffer;

		// connecting new websocket
		if (header->header == 10001) {
			RequestConnection* r = (RequestConnection*)Info.buffer;
			defineClient(Info, r->name, 1, r->user);

			std::cout << "WEB SOCKET is Connecting: " << r->name << std::endl;

			return 0;
		}
		
		// sending command to device
		if (header->header == 10020) {
			RCommand* r = (RCommand*)Info.buffer;

			SOCKET deviceSocket = getSocket(r->unitId);

			auto found = clients.find(deviceSocket);

			RCommand response;
			response.header = 10021;
			response.mode = r->mode;
			response.type = r->type;
			response.id = r->id;
			response.unitId = r->unitId;
			response.level = r->level;
			
			strcpy_s(response.unit, sizeof response.unit, r->unit);
			strcpy_s(response.user, sizeof response.user, r->user);

			if (found == clients.end()) {

				std::string message = "SENDING TO NOTHING";
				std::cout << "Sending message: " << r->message << " to Nothing\n\n";

				strcpy_s(response.message, sizeof response.message, message.c_str());
			}
			else {

				sendCommand(r->unitId, std::string(r->message));

				strcpy_s(response.message, sizeof response.message, r->message);
			}
			char buffer[1024];

			memcpy(buffer, &response, sizeof(response));

			send(socket, buffer, (int)sizeof(buffer), 0);

			return 0;
		}

		// disconnecting a device from socket
		if (header->header == 10100) {
			RCommand* r = (RCommand*)Info.buffer;

			SOCKET deviceSocket = getSocket(r->unitId);

			auto found = clients.find(deviceSocket);
			if (found == clients.end()) {
				return 0;
			}


			RCommand response;
			response.header = 10021;
			response.mode = r->mode;
			response.type = r->type;
			response.id = r->id;
			response.unitId = r->unitId;
			response.level = r->level;
			strcpy_s(response.unit, sizeof response.unit, r->unit);
			strcpy_s(response.user, sizeof response.user, r->user);
			strcpy_s(response.message, sizeof response.message, r->message);

			//std::string str = r->message;
			
			//strcpy_s(response.message, sizeof response.message, str.c_str());
			//strcpy(response.message, str.c_str());

			
			char buffer[1024];
			memcpy(buffer, &response, sizeof(response));
			send(socket, buffer, (int)sizeof(buffer), 0);


			//send(mDevices[getUnitName(r->unitId)].socket, r->message, strlen(r->message), 0);

			std::cout << Color::_green() << "Sending: " << response.message
				<< Color::_reset() << " to: "
				<< r->name << " (" << r->unit << ")\n";
			/*
			std::cout << "Header: " << response.header << std::endl;
			std::cout << "Message: " << response.message << std::endl;
			std::cout << "UnitId: " << response.unitId << std::endl;
			std::cout << "Mode: " << response.mode << std::endl;
			std::cout << "User: " << response.user << std::endl;
			std::cout << "Unit: " << response.unit << std::endl;
			*/

			closeClient(deviceSocket);
			std::cout << "DISCONECTING TO: " << deviceSocket << "\n";
			
			return 0;

		}

		return header->header;
	}

	int Server::returnMessage(RCommand & unitResponse) {
		

		unitResponse.header = 0;


		time_t now;
		time(&now);


		unitResponse.delay = difftime(now, unitResponse.time);
		//std::cout << Color::_magenta() << " delay Time: " << unitResponse.delay << Color::_reset() << std::endl;


		//db->saveResponse(&unitResponse, to.c_str());

		//time_t rawtime;
		struct tm* timeinfo;
		time(&now);
		timeinfo = localtime(&now);

		

		
		
		unitResponse.typeMessage = ClientMsg::CommandResponse;
		broadcast(&unitResponse);
	}

	bool Server::sendCommand(int id, std::string message)
	{
		SOCKET targetSocket = getSocket(id);
		if (targetSocket > 0) {
			send(targetSocket, message.c_str(), message.size(), 0);
			return true;
		}
		return false;
	}
	


	
	void Server::closeClient(SOCKET client) {

		disconnect(client);
		
		deleteClient(client);
		
	}
	
	void Server::deleteClient(SOCKET client) {

		std::cout << "ON CLOSE ID: " << client << " \n";
		
		
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
		

		if (resp.unitId > 0) {
			

			InfoEvent event;
			event.unitId = clients[client].id;
			event.eventId = 202;
			event.mode = 0;
			event.info = "";
			event.title = "disconnected";

			insertEvent(event);


		}
		m.lock();
		clients.erase(client);
		broadcast(&resp);
		m.unlock();

		
	}
	
	void Server::onClose(ConnInfo Info) {
		
		
		deleteClient(Info.client);
		

	}
	
	
	void Server::broadcast(RCommand * response) {
		//cout << endl << "***** *** Entrando a un Broadcast; " << rClients.size() << endl;
		
		char buffer[512];
		
		response->header = 10050;

		memcpy(buffer, &(*response), sizeof(RCommand));
		
		for (std::map<SOCKET, GTClient1>::iterator it = clients.begin(); it != clients.end(); ++it) {
			if (it->second.type == 1) {
				send(it->second.socket, buffer, (int)sizeof(buffer), 0);
			}
			
		}
		
		memset(&buffer, 0, sizeof(buffer));//clear the buffer
		
	}
	
	

	SOCKET Server::getSocket(int id) {
		
		for (std::map<SOCKET, GTClient1>::iterator it = clients.begin(); it != clients.end(); ++it) {
			
			if (it->second.id == id) {
				return it->first;
			}
		}
		return 0;
	}
	
	bool Server::saveTrack(int unitId, int formatId, const char* buffer) {
		m2.lock();
		bool result = true;// db->saveTrack(unitId, formatId, buffer);
		m2.unlock();

		return result;
		
	}
	
	bool Server::isVersion(int value)
	{
		m2.lock();
		//bool result = db->isVersion(value);
		m2.unlock();
		return true;// result;
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
		int result = true;// db->updateCommand(unitId, commandId, index, mode, params);
		m2.unlock();

		return result;
	}
	
	void Server::getIndexCommand(const char* unit_id, CommandResult* commandResult, RCommand* info)
	{
		m2.lock();
		//*****db->getIndexCommand(unit_id, commandResult, info);
		m2.unlock();
	}
	
	void Server::infoCommand(const char* unit_id, CommandResult* commandResult, RCommand* info)
	{
		m2.lock();
		//*****db->infoCommand(unit_id, commandResult, info);
		m2.unlock();
	}
	
	void Server::insertTrack(std::string name, std::string track)
	{
		m2.lock();
		webcar->insertTrack(name, track.c_str());
		m2.unlock();
		return;
	}

	void Server::wcTracking(int unitId, std::map<std::string, std::string> & data)
	{
		m2.lock();

		webcar->saveTracking(unitId, data);
		
		m2.unlock();
		return;
	}
	
	bool Server::insertEvent(InfoEvent &infoEvent)
	{
		m2.lock();
		db->insertEvent(infoEvent);
		m2.unlock();
		return true;
	}



	SOCKET Server::getSocket(std::string name) {
		
		for (std::map<SOCKET, GTClient1>::iterator it = clients.begin(); it != clients.end(); ++it) {
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
		
		std::map<SOCKET, GTClient1> cli = clients;
		for (std::map<SOCKET, GTClient1>::iterator it = cli.begin(); it != cli.end(); ++it) {
			
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
			printf("%18s", it->second.address.c_str());
			printf("%12s", it->second.name.c_str());
			//printf("%6d", it->second.header);
			printf("%12.3f", timeInSeconds);
			printf("%12.3f", delta);
			printf("%8d", int(it->second.socket));
			printf("%8d", it->second.verionId);
			printf("%6d\n", it->second.type);
			
			if (it->second.type == 0 && timeInSeconds > (double)waitTime) {
				printf("%50s\n", "-- DISCONECTING TO UNKNOWN && waitTime: %d", waitTime);
				//printf("%10d", it->second.header);
				printf("%3d", n);
				printf("%6d", it->second.id);
				printf("%18s", it->second.address.c_str());
				printf("%12s", it->second.name.c_str());
				//printf("%6d", it->second.header);
				printf("%12.3f", timeInSeconds);
				printf("%12.3f", delta);
				printf("%8d", int(it->second.socket));
				printf("%8d", it->second.verionId);
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
				printf("%18s", it->second.address.c_str());
				printf("%12s", it->second.name.c_str());
				//printf("%6d", it->second.header);
				printf("%12.3f", timeInSeconds);
				printf("%12.3f", delta);
				printf("%8d", int(it->second.socket));
				printf("%8d", it->second.verionId);
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