#define _CRT_SECURE_NO_WARNINGS
#include "WebServer.h"
using namespace rapidjson;
using namespace std;

//std::mutex m2;
//std::mutex m3;
std::mutex m1;
//std::mutex m4;
//std::mutex m5;

namespace GT {

	void runTimer() {
		setlocale(LC_CTYPE, "Spanish");

		while (true) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
			auto t = std::time(nullptr);
			auto tm = *std::localtime(&t);
			//std::cout << std::put_time(&tm, "%d/%m/%Y %H:%M:%S") << std::endl;
			//std::cout << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << std::endl;
		}
	}

	HubConfig getHugConfig(rapidjson::Value& d)
	{
		HubConfig info;
		info.name = d["name"].GetString();
		info.host = d["host"].GetString();
		info.port = d["port"].GetInt();
		

		return info;
	}

	WebServer::WebServer(SocketInfo pInfo):
		WebSocketServer(pInfo),
		
		hub(nullptr) {
	}
	
	void WebServer::init() {

		rapidjson::Document d = loadConfig("wsserver.json");


		info.port = d["port"].GetInt();
		info.maxClients = d["max_clients"].GetInt();
		

		InfoDB infoDB = {

			d["db"]["host"].GetString(),
			d["db"]["port"].GetString(),
			d["db"]["name"].GetString(),
			d["db"]["user"].GetString(),
			d["db"]["pass"].GetString(),
			false

		};
		db = new ProtoDB(infoDB);
		db->connect();
		std::map<std::string, HubConfig> mConfig;
		const rapidjson::Value& a = d["hubs"];

		for (rapidjson::SizeType i = 0; i < a.Size(); i++) {
			rapidjson::Value& pp = d["hubs"][i];
			HubConfig infoConfig = getHugConfig(pp);
			mConfig[infoConfig.name] = infoConfig;
		}

		std::vector<std::thread*> tasks;

		for (std::map<std::string, HubConfig>::iterator it = mConfig.begin(); it != mConfig.end(); ++it) {
			std::cout << "App: " << it->second.name << ", ";
			std::cout << "Port:" << it->second.port << "\n";
			tasks.push_back(new std::thread(runHub, &it->second, this));
		}

		
		start();

		for (std::vector<std::thread*>::iterator it = tasks.begin(); it != tasks.end(); ++it) {
			(*it)->join();
		}

	}

	void WebServer::onConnect(ConnInfo Info) {
		
	}

	/* Message from web app */
	void WebServer::onMessage(ConnInfo Info) {

		WebSocketServer::onMessage(Info);

		std::string message = decodeMessage(Info);

		std::cout << "Message:[" << message << "]\n";

		Document doc;
		doc.Parse(message.c_str());

		if (!doc.IsObject()) {
			printf("unknown message: %s", message.c_str());
			return;
		}

		std::string type = "";

		if (doc.HasMember("type")) {
			type = doc["type"].GetString();
		}


		if (type == "connect") {

			ConnectInfo info = getConnectInfo(doc);
			std::cout << "name: " << info.name << " user: " << info.user << "\n";

			startClient(info);


		}

		if (type == "cmd") {
			CommandInfo info = getCommandInfo(doc);
			std::cout << "command: " << info.command << "\n";

			db->saveCmd({
				info.unitId,
				info.command,
				info.index,
				info.mode,
				info.status,
				info.name,
				info.params,
				info.query,
				info.values,
				info.user

			});
			evalCommand(info);
			
			if (info.status == 2) {
				EncodeCommand encodeInfo = db->infoCommand(info.unitId, info.command, info.index);


				std::map<std::string, std::string> cmdData;
				cmdData["id"] = encodeInfo.deviceId;
				cmdData["pass"] = encodeInfo.pass;
				cmdData["cmd"] = encodeInfo.command;


				cmdData["fpacklen"] = encodeInfo.packlen;
				cmdData["fchecksum"] = encodeInfo.checksum;
				cmdData["index"] = std::to_string(encodeInfo.index);
				cmdData["oparams"] = encodeInfo.listOParams;
				cmdData["params"] = encodeInfo.listParams;
				cmdData["packno"] = encodeInfo.packno;


				Cmd cmd;
				std::string strCommand = cmd.encode(encodeInfo.commandExp, cmdData);

				std::cout << "new command es : " << encodeInfo.commandExp << strCommand << "\n\n";

				GT::RCommand r = {
					10020,
					info.mode,
					Info.client,
					"",
					"",
					"",
					"",//name
					info.unitId,
					0,
					0,
					"",// date
					1,
					info.index,//index
					ClientMsg::Request,
					0,// time
					0,// Delay
					info.index,
					0
				};

				strcpy_s(r.message, sizeof r.message, strCommand.c_str());
				strcpy_s(r.command, sizeof r.command, "");
				strcpy_s(r.user, sizeof r.user, info.user.c_str());

				sendCommand(r);
			}
			else {
				std::cout << "SAVING COMMAND FOR LATE...\n";
			}

			


		}

		if (type == "rc") {

		}


		return;

		//m4.lock();

		Document f;
		f.SetObject();
		Value xx;
		xx = 100;

		Value aa(kArrayType);
		Document::AllocatorType& allocator = f.GetAllocator();
		aa.PushBack(Value().SetInt(47), allocator);
		aa.PushBack(Value(37).Move(), allocator);

		StringBuffer bf4;
		PrettyWriter<StringBuffer> writer4(bf4);
		f.Accept(writer4);

		//std::cout << "--> A: " << bf4.GetString() << " \n<--" << std::endl;




		//cout << Info.buffer << endl;



		//SOCKET s = hub->getHost();

		Document document;
		document.Parse("");



		if (!document.IsObject()) {

			//printf("Connecting !\n");
			//m4.unlock();
			return;
		}



		//const Value& values = document["comdValues"].GetArray();
	   // cout << values[0].GetInt() << endl;
		//document["comdValues"].GetArray();

		//printf("JSON deviceId %d\n", document["deviceId"].GetInt());

		//std::cout <<"Type 1: "<< document["type"].GetString() << std::endl;

		string msgType = document["type"].GetString();

		if (msgType == "CS") {
			/*
			for (auto i = document.MemberBegin(); i != document.MemberEnd(); ++i)
			{
				std::cout << "key: " << i->name.GetString() << " : " << i->value.IsInt() << std::endl;
				//WalkNodes(i->value);
			}
			*/
			if (document["unitId"].IsInt() &&
				document["commandId"].IsInt() &&
				document["index"].IsInt() &&
				document["mode"].IsInt()) {
				int unitId = document["unitId"].GetInt();
				int commandId = document["commandId"].GetInt();
				int index = document["index"].GetInt();
				int mode = document["mode"].GetInt();
				std::string user = "";
				if (document.HasMember("user")) {
					user = document["user"].GetString();
				}

				sendToDevice(Info, unitId, commandId, index, mode, user);
			}
			//sendCommand(unitId, commandId, index, mode);
		//m4.unlock();
			return;
		}

		if (msgType == "RC") {
			//std::cout << "RC\n\n";

			for (auto i = document.MemberBegin(); i != document.MemberEnd(); ++i)
			{
				//std::cout << "key: " << i->name.GetString() << " : " << i->value.IsInt() << std::endl;
				//WalkNodes(i->value);
			}
			if (document["unitId"].IsInt()) {
				int unitId = document["unitId"].GetInt();

				GT::RCommand r = {
					//10020,
					10100,
					10,
					Info.client,
					"",
					"",
					"",
					"",//name
					unitId,
					0,
					0,
					"",// date
					0,
					0,//index
					ClientMsg::Request,
					0,// time
					0,// Delay
					0
				};

				sendToDevice(hub->getHost(), &r);
			}
			//sendCommand(unitId, commandId, index, mode);
			//m4.unlock();
			return;
		}



		if (msgType == "connect") {
			/*
			for (auto i = document.MemberBegin(); i != document.MemberEnd(); ++i)
			{
				//std::cout << "key: " << i->name.GetString() << " : " << i->value.IsInt() << std::endl;
				//WalkNodes(i->value);
			}
			*/
			clients[Info.client].socket = Info.client;
			std::string name = document["name"].GetString();
			std::string user = document["user"].GetString();

			strcpy_s(clients[Info.client].name, sizeof(clients[Info.client].name), name.c_str());
			strcpy_s(clients[Info.client].user, sizeof(clients[Info.client].user), user.c_str());




			//cout << "connecting: " << clients[Info.client].name << endl;

			//cout << "Name: " << name << endl;
			//cout << "User: " << name << endl;


			char buffer[DEFAULT_BUFLEN];
			size_t size = 0;
			encodeMessage((char*)document["clientName"].GetString(), buffer, size);

			//printf("%s(%d)\n", Info.buffer, size);

			send(Info.client, buffer, (int)size, 0);
			//m4.unlock();
			return;
		}

		//m4.unlock();


	}

	void WebServer::test2(unsigned int socket) {
		string str = "yanny esteban";
		char buffer[512];
		size_t size;
		encodeMessage((char*)str.c_str(), buffer, size);

		send(socket, buffer, size, 0);
		cout << "jamas" << endl;
	}

	WebClient WebServer::getClient(void) {

		/*
		for (std::map<SOCKET, WebClient>::iterator it = clients.begin(); it != clients.end(); ++it) {

			printf("%40s", it->second.name);
			

		}
		*/
		return WebClient();
	}

	unsigned short WebServer::getHeader(char * buffer) {

		IdHeader* header = (IdHeader*)buffer;
		std::string command = "";

		//std::cout << ANSI_COLOR_MAGENTA "Real Header: " << header->header << endl;
		if (header->header == 10050) {
		
			RCommand* response = (RCommand*)buffer;
			std::cout << ANSI_COLOR_YELLOW "Message Receive: " << response->message << std::endl;
			//cout << " TEST " << clients.size() << "........" << endl;
			for (std::map<SOCKET, WebClient>::iterator it = clients.begin(); it != clients.end(); ++it) {
				//cout << " --- Name: " << it->second.name << endl;
				//cout << " --- socket: " << it->first << endl;
				string str = response->message;
				char buffer2[512];
				size_t size2;
				encodeMessage((char*)str.c_str(), buffer2, size2);

				send(it->first, buffer2, size2, 0);
				jsonResponse(it->first, response);

				//encodeMessage((char*)"yanny esteban", buffer2, size2);

				//send(it->first, buffer2, size2, 0);

			}


			/*
			*/
			return 1;
		
		}

		if (header->header == 10021) {
			RCommand* response = (RCommand*)buffer;
			//std::cout << ANSI_COLOR_YELLOW "10021 Header: " << response->header << std::endl;
			std::cout << "Message Receive: " << response->message << "\n" << std::endl;
			
			//std::cout << "UnitId: " << response->unitId << std::endl;
			//std::cout << "Mode: " << response->mode << std::endl;
			//std::cout << "User: " << response->user << std::endl;
			//std::cout << "Unit: " << response->unit << std::endl;
			//std::cout << "ID: " << response->id << std::endl;
			//printf(ANSI_COLOR_RED "...%s....\n" ANSI_COLOR_RESET, response->unit);
			//getClient();
			//send(r->id, "yet 2030", 8, 0);
			//WS->test2(r->id);


			string str = response->message;
			char buffer2[512];
			size_t size2;
			encodeMessage((char*)str.c_str(), buffer2, size2);

			send(response->id, buffer2, size2, 0);
			
			return 1;
		}


		return 0;
	}

	void WebServer::jsonResponse(SOCKET client, RCommand * response) {
		Document json;
		json.SetObject();
	   

		Value msg;
		//char buffer[512];
		//int len = sprintf(buffer, "%s", response->message);
		msg.SetString(response->message, strlen(response->message), json.GetAllocator());
		json.AddMember("message", msg, json.GetAllocator());

		//strcpy_s(buffer, strlen(response->unit), response->unit);
		msg.SetString(response->unit, strlen(response->unit), json.GetAllocator());
		json.AddMember("unit", msg, json.GetAllocator());
		msg.SetString(response->date, strlen(response->date), json.GetAllocator());
		json.AddMember("date", msg, json.GetAllocator());


		msg.SetString(response->name, strlen(response->name), json.GetAllocator());
		json.AddMember("name", msg, json.GetAllocator());

		msg.SetString(response->user, strlen(response->user), json.GetAllocator());
		json.AddMember("user", msg, json.GetAllocator());
		
		json.AddMember("header", response->header, json.GetAllocator());
		
		json.AddMember("unitId", response->unitId, json.GetAllocator());
		json.AddMember("commandId", response->commandId, json.GetAllocator());
		json.AddMember("index", response->index, json.GetAllocator());
		json.AddMember("delay", response->delay, json.GetAllocator());
		json.AddMember("type", (int)response->typeMessage, json.GetAllocator());
		json.AddMember("roleId", response->roleId, json.GetAllocator());
		//Value v;
		//v.SetString(response->user, strlen(response->user), z.GetAllocator());
		//z["user"].SetString(v);
			//z["user"].SetString(response->user, strlen(response->user), z.GetAllocator());
		//z["unit"].SetString(response->unit, strlen(response->unit), z.GetAllocator());
		//z["date"].SetString(response->date, strlen(response->date), z.GetAllocator());
	  
			

		StringBuffer bf5;
		Writer<StringBuffer> writer5(bf5);

		json.Parse(bf5.GetString());

		json.Accept(writer5);

		//std::cout << "Return to Wepapp: \n";// << bf5.GetString() << std::endl;



		string str = response->message;
		char buffer2[512];
		size_t size2;
		encodeMessage((char*)bf5.GetString(), buffer2, size2);

		send(client, buffer2, size2, 0);
	
	}

	void WebServer::sendCommand(int unitId, int commandId, int index, int mode) {
		std::cout << " unit Id " << unitId << std::endl;
		std::cout << " commandId  " << commandId << std::endl;
		std::cout << " index " << index << std::endl;
		std::cout << " mode " << mode << std::endl;
		//std::cout << "......" << db->loadCommand(unitId, commandId, index, mode) << std::endl;
	}

	void WebServer::sendToDevice(ConnInfo Info, int unitId, int commandId, int index, int mode, std::string user) {
		

		GT::RCommand r = {
			//10020,
			10020,
			mode,
			Info.client,
			"",
			"",
			"",
			"",//name
			unitId,
			commandId,
			mode,
			"",// date
			1,
			index,//index
			ClientMsg::Request,
			0,// time
			0,// Delay
			index,
			0
		};


		
		string role = "";
		int roleId = 0;

		printf("UnitId: %d, CommandId: %d, index: %d, mode: %d, role: %s, ", unitId, commandId, index, mode, role.c_str());

		db->isAlive();
		std::string strCommand = "x";// db->loadCommand(unitId, commandId, index, mode, role, roleId);


		strcpy_s(r.message, sizeof r.message, strCommand.c_str());
		strcpy_s(r.command, sizeof r.command, role.c_str());
		strcpy_s(r.user, sizeof r.user, user.c_str());
		printf("\ncommand: %s\n\n", strCommand.c_str());
		
		
		char buffer2[1024];
		memcpy(buffer2, &r, sizeof(r));


		DBEvent event;
		event.unitId = unitId;
		event.eventId = 209;
		//strftime(event.dateTime, sizeof(event.dateTime), "%F %T", timeinfo);
		strcpy_s(event.title, sizeof event.title, r.command);
		strcpy_s(event.user, sizeof event.user, r.user);
		strcpy_s(event.info, sizeof(r.message), r.message);
		//strcpy(event.info, "");
		//db->insertEvent(&event);

		sendToServers(buffer2, (int)sizeof(buffer2));
		//send(Info.client, "yanny", strlen("yanny"), 0);

		char buffer[DEFAULT_BUFLEN];
		size_t size = 0;

		RCommand response;
		response.unitId = r.unitId;
		response.commandId = r.commandId;

		//char message[100] = "Receiving ";
		//strcat_s(message, sizeof(message), r.message);
		strcpy_s(response.message, sizeof(response.message), r.message);
		strcpy_s(response.user, sizeof(response.user), r.user);
		strcpy_s(response.name, sizeof(response.name), r.unit);
		strcpy_s(response.unit, sizeof(response.unit), "Sending...");
		//strcpy_s(response., sizeof(response.name), r.unit);

		time_t rawtime;
		struct tm* timeinfo;


		time(&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(response.date, sizeof(response.date), "%F %T", timeinfo);



		response.unitId = r.unitId;
		response.roleId = roleId;
		jsonResponse(Info.client, &response);
		//m5.unlock();
		/*

		encodeMessage((char*)"websocket 2021", buffer, size);

		printf("%s(%d)\n", Info.buffer, size);

		send(Info.client, buffer, (int)size, 0);
	*/
	}

	void WebServer::sendToDevice(SOCKET server, GT::RCommand* request)
	{
		
		
		
		std::string msg = "disconecting";
		strcpy_s(request->message, sizeof request->message, msg.c_str());
		strcpy_s(request->command, sizeof request->command, msg.c_str());

		cout << endl << "Unidad" << request->unitId << endl << "COMANDO " << msg << endl << endl;


		
		char buffer2[1024];

		std::cout << " REQUEST " << sizeof(request) << "\n\n";
		std::cout << " REQUEST " << sizeof(GT::RCommand) << "\n\n";

		memcpy(buffer2, request, sizeof(GT::RCommand));
		//send(server, buffer2, (int)sizeof(buffer2), 0);
		sendToServers(buffer2, (int)sizeof(buffer2));

		return;
		//send(Info.client, "yanny", strlen("yanny"), 0);

		char buffer[DEFAULT_BUFLEN];
		size_t size = 0;

		RCommand response;
		response.unitId = request->unitId;
		response.commandId = request->commandId;

		//char message[100] = "Receiving ";
		//strcat_s(message, sizeof(message), r.message);
		strcpy_s(response.message, sizeof(response.message), request->message);
		strcpy_s(response.user, sizeof(response.user), request->user);
		strcpy_s(response.name, sizeof(response.name), request->unit);
		strcpy_s(response.unit, sizeof(response.unit), "Sending...");
		//strcpy_s(response., sizeof(response.name), r.unit);

		time_t rawtime;
		struct tm* timeinfo;


		time(&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(response.date, sizeof(response.date), "%F %T", timeinfo);



		response.unitId = request->unitId;
		//jsonResponse(Info.client, &response);

	}

	void WebServer::sendCommand(GT::RCommand rCommand)
	{
		char buffer[1024];
		memcpy(buffer, &rCommand, sizeof(rCommand));
		sendToServers(buffer, (int)sizeof(buffer));
	}

	int WebServer::sendToServers(char* buffer, int len)
	{
		

		//std::map<std::string, Hub*> hubs;
		

		for (std::map<std::string, Hub*>::iterator it = hubs.begin(); it != hubs.end(); ++it) {
			
		
			send(it->second->getHost(), buffer, len, 0);
		}
		
		return 0;
	}

	ConnectInfo WebServer::getConnectInfo(const rapidjson::Document& doc)
	{
		ConnectInfo info;
		//Document doc;
		//doc.Parse(json.c_str());

		

		if (doc.HasMember("name") && doc["name"].IsString()) {
			info.name = doc["name"].GetString();
		}
		if (doc.HasMember("user") && doc["user"].IsString()) {
			info.user = doc["user"].GetString();
		}
		
		return info;
	}

	CommandInfo WebServer::getCommandInfo(const rapidjson::Document& doc)
	{
		
		
		CommandInfo info;
		//Document doc;
		//doc.Parse(json.c_str());

		if (doc.HasMember("unitId") && doc["unitId"].IsInt()) {
			info.unitId = doc["unitId"].GetInt();
		}
		if (doc.HasMember("index") && doc["index"].IsInt()) {
			info.index = doc["index"].GetInt();
		}
		if (doc.HasMember("mode") && doc["mode"].IsInt()) {
			info.mode = doc["mode"].GetInt();
		}
		if (doc.HasMember("status") && doc["status"].IsInt()) {
			info.status = doc["status"].GetInt();
		}

		if (doc.HasMember("command") && doc["command"].IsString()) {
			info.command = doc["command"].GetString();
		}
		if (doc.HasMember("name") && doc["name"].IsString()) {
			info.name = doc["name"].GetString();
		}
		if (doc.HasMember("params") && doc["params"].IsString()) {
			info.params = doc["params"].GetString();
		}
		if (doc.HasMember("query") && doc["query"].IsString()) {
			info.query = doc["query"].GetString();
		}
		if (doc.HasMember("values") && doc["values"].IsString()) {
			info.values = doc["values"].GetString();
		}
		if (doc.HasMember("user")) {
			info.user = doc["user"].GetString();
		}

		return info;
	}

	void WebServer::startClient(ConnectInfo)
	{
	}

	void WebServer::evalCommand(CommandInfo)
	{
	}

	std::string WebServer::loadCommand(int unitId, int commandId, int index, int mode, std::string& role, int & roleId)
	{
		//m2.lock();
		std::string strCommand = "";// db->loadCommand(unitId, commandId, index, mode, role, roleId);
		//m2.unlock();
		return strCommand;
	}

	bool WebServer::insertEvent(DBEvent* infoEvent)
	{
		
		//m2.lock();
		//db->insertEvent(infoEvent);
		//m2.unlock();
		return true;
	   
	}

	
	
	
	

	void WebServer::send2(char* message) {
		char buffer[DEFAULT_BUFLEN];
		size_t size = 0;
		encodeMessage(message, buffer, size);

		//printf("%s(%d)\n", Info.buffer, size);

		//send(Info.client, buffer, (int)size, 0);
	}

	

}


void CallReceive(void * app, char* buffer, size_t size) {

	GT::RCommand* x = (GT::RCommand*)buffer;


	cout << "Client Message" << endl;

	GT::WebServer* WS = (GT::WebServer*)app;

	if (WS->getHeader(buffer) == 0) {
		cout << "ERROR" << endl;
	}

	
	
}

void CallConection(GT::CSInfo Info) {

	std::cout << " CallConection to Socket Server: " << Info.name << "\n\n";

	GT::RequestConnection c = {
		10001,
		Info.master,
		"hub5",
		"panda-x5",
		1,
		-2,
		"hola dejame entrar",
		0


	};

	strcpy_s(c.name, sizeof(c.name), (char*)Info.name);

	char buffer2[512];
	memcpy(buffer2, &c, sizeof(c));
	send(Info.master, buffer2, sizeof(buffer2), 0);

}

BOOL __stdcall mainhub(LPVOID param) {
	GT::WebServer* WS = (GT::WebServer*)param;

	GT::CSInfo Info = { 0, 0, 0, 0, 0, 0, (char *) "", 0, (char*)"", (char*)"", (char*)"", 0,(char*)"" };

	
	Info.host = (char *)WS->configInit.hub.host;
	Info.port = WS->configInit.hub.port;// std::to_string(WS->configInit.hub.port).c_str();
	


   
	Info.name = (char *)WS->configInit.name;

	std::cout << "el puerto del hug es " << WS->configInit.hub.port << endl << endl;
	std::cout << "Activate the HUB server " << endl;
	std::cout << "Name " << WS->configInit.name << endl;
	WS->hub = new GT::Hub(Info);
	WS->hub->appData = WS;
	WS->hub->CallConection = CallConection;
	WS->hub->callReceive = CallReceive;
	std::cout << ANSI_COLOR_RESET "Asking for Connection..." << WS->configInit.port << endl;
	while (WS->reconnect) {
		WS->hub->start();
		std::cout << ANSI_COLOR_RESET "Reconnecting..."<< WS->configInit.port << endl;
		Sleep(5000);
	}

	std::cout << "Godd Bye" << endl;
	return true;
}

rapidjson::Document loadConfig(const char* path)
{


	FILE* fp = fopen(path, "rb"); // non-Windowsyannyesteban@ho use "r"

	if (fp == NULL)
	{
		perror("Error while opening the file.\n");
		exit(EXIT_FAILURE);
	}

	char* readBuffer;

	readBuffer = (char*)malloc(1500);
	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	rapidjson::Document d;

	d.ParseStream(is);
	fclose(fp);


	if (readBuffer != NULL) {
		readBuffer[0] = '\0';
		free(readBuffer);
	}
	return d;
}



void runHub(HubConfig* config, LPVOID param/*LPVOID param, HubConfig* c*/)
{
	
	m1.lock();
	GT::WebServer* WS = (GT::WebServer*)param;

	GT::CSInfo Info = { 0, 0, 0, 0, 0, 0, (char*)"", 0, (char*)"", (char*)"", (char*)"", 0,(char*)"" };

	Info.host = (char*)config->host;
	Info.port = config->port;// std::to_string(WS->configInit.hub.port).c_str();
	Info.name = (char*)config->name;

	
	std::cout << "\nConnecting to: " << config->name << " on Port: "<< config->port << endl;
   
	auto hub = new GT::Hub(Info);
	hub->appData = WS;
	hub->CallConection = CallConection;
	hub->callReceive = CallReceive;

	WS->hubs[config->name] = hub;
	WS->nHubs++;
	//std::cout << "Size "<< WS->nHubs << "- - -" << WS->hubs.size() << endl;
	//std::cout << ANSI_COLOR_RESET "Asking for Connection..." << config->port << endl;
	m1.unlock();
	while (WS->reconnect) {
		hub->start();
		std::cout << ANSI_COLOR_RESET "Reconnecting..." << config->port << endl;
		Sleep(5000);
	}

	//std::cout << "Godd Bye" << endl;
}
