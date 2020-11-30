#define _CRT_SECURE_NO_WARNINGS
#include "WebServer.h"
using namespace rapidjson;
using namespace std;

namespace GT {
    WebServer::WebServer(SocketInfo pInfo):
        WebSocketServer(pInfo),
        
        hub(nullptr) {
    }
    void WebServer::init() {


        //auto appInfo = GT::Config::load("config.json");
        this->configInit = GT::JsonConfig::load("wsserver.json");

        auto appInfo = this->configInit;// GT::JsonConfig::load("wsserver.json");
        //this->configInit = &appInfo;
        cout << "APP: ** " <<appInfo.db.name << endl;
        cout << "Hub port: " << appInfo.hub.port << endl;
        //config = pConfig;
        // pConfig.db.debug = pConfig.debug;
        db = new DB(appInfo.db);
        db->connect();


        info.port = configInit.port;
        info.maxClients = configInit.max_clients;


        hClientThread = CreateThread(
            NULL,
            0,
            (LPTHREAD_START_ROUTINE)mainhub,
            this,
            0,
            &dwThreadId);
       
	}

    void WebServer::onConnect(ConnInfo Info) {
        puts("onConnect 2020");
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


        for (std::map<SOCKET, WebClient>::iterator it = clients.begin(); it != clients.end(); ++it) {

            printf("%40s", it->second.name);
            

        }
        return WebClient();
    }

    unsigned short WebServer::getHeader(char * buffer) {

        IdHeader* header = (IdHeader*)buffer;
        std::string command = "";

        std::cout << ANSI_COLOR_MAGENTA "Real Header: " << header->header << endl;
        if (header->header == 10050) {
        
            RCommand* response = (RCommand*)buffer;
            std::cout << ANSI_COLOR_YELLOW "10050 Message: " << response->message << std::endl;

            for (std::map<SOCKET, WebClient>::iterator it = clients.begin(); it != clients.end(); ++it) {
                cout << " --- Name: " << it->second.name << endl;
                cout << " --- socket: " << it->first << endl;
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
            std::cout << ANSI_COLOR_YELLOW "10021 Header: " << response->header << std::endl;
            std::cout << "Message: " << response->message << std::endl;
            
            std::cout << "UnitId: " << response->unitId << std::endl;
            std::cout << "Mode: " << response->mode << std::endl;
            std::cout << "User: " << response->user << std::endl;
            std::cout << "Unit: " << response->unit << std::endl;
            std::cout << "ID: " << response->id << std::endl;
            printf(ANSI_COLOR_RED "...%s....\n" ANSI_COLOR_RESET, response->unit);
            getClient();
            //send(r->id, "yet 2030", 8, 0);
            //WS->test2(r->id);


            string str = response->message;
            char buffer2[512];
            size_t size2;
           encodeMessage((char*)str.c_str(), buffer2, size2);

            send(response->id, buffer2, size2, 0);
            //cout << "jamas" << endl;
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
        json.AddMember("delay", response->delay, json.GetAllocator());
        json.AddMember("type", (int)response->typeMessage, json.GetAllocator());
        
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

        std::cout << "A5: " << bf5.GetString() << std::endl;



        string str = response->message;
        char buffer2[512];
        size_t size2;
        encodeMessage((char*)bf5.GetString(), buffer2, size2);

        send(client, buffer2, size2, 0);
    
    }
    /* Message from web app */
    void WebServer::onMessage(ConnInfo Info) {


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

        std::cout << "A: " << bf4.GetString() << std::endl;

       


        //cout << Info.buffer << endl;

        WebSocketServer::onMessage(Info);
        
        const char* x = decodeMessage(Info);
        
        SOCKET s = hub->getHost();

        Document document;
        document.Parse(x);
        if (!document.IsObject()) {
            printf(" ** ** **     **  ERROR JSON...!!!\n");
            return;
        }


        
        //const Value& values = document["comdValues"].GetArray();
       // cout << values[0].GetInt() << endl;
        //document["comdValues"].GetArray();

        //printf("JSON deviceId %d\n", document["deviceId"].GetInt());
        
        //std::cout <<"Type 1: "<< document["type"].GetString() << std::endl;

        string msgType = document["type"].GetString();
        //string msgName = document["name"].GetString();
        //std::cout << "name " << document["name"].GetString() << endl;
        unsigned short type = 0;

        if (msgType == "connect") {

            clients[Info.client].socket = Info.client;
            std::string name = document["clientName"].GetString();
            
            strcpy_s(clients[Info.client].name, sizeof(clients[Info.client].name), name.c_str());
            
            

            type = 1;
            cout << "connecting: " << clients[Info.client].name << endl;

            cout << "client Name" << document["clientName"].GetString() << endl;


            char buffer[DEFAULT_BUFLEN];
            size_t size = 0;
            encodeMessage((char *)document["clientName"].GetString(), buffer, size);

            //printf("%s(%d)\n", Info.buffer, size);

            send(Info.client, buffer, (int)size, 0);
            return;
        }

        const Value& values = document["comdValues"].GetArray();
        std::list<string> params;

        //assert(attributes.IsArray()); // attributes is an array
        //for (rapidjson::Value::ConstValueIterator itr = values.Begin(); itr != values.End(); ++itr) {
        //    const rapidjson::Value& attribute = *itr;
        //    assert(attribute.IsObject()); // each attribute is an object
        //    for (rapidjson::Value::ConstMemberIterator itr2 = attribute.MemberBegin(); itr2 != attribute.MemberEnd(); ++itr2) {
        //        //std::cout << itr2->name.GetString() << " : " << std::endl;
        //        std::cout << itr2->name.GetString() << " : " << itr2->value.GetString() << std::endl;
        //    }
        //}


        for (SizeType i = 0; i < values.Size(); i++) {
            std::cout << "         hooolaaaaaaaaa " << std::endl;
            params.push_back(values[i].GetString());
            std::cout << " Array " << i << " es " << values[i].GetString() << std::endl;
        }

        for (std::list<std::string>::iterator it = params.begin() ; it != params.end(); ++it) {
            std::cout << " Array \t" << *it <<  std::endl;
        }
        
        //cout << values[0].GetString() << endl;




        if (msgType == "SET") {
            type = 1;
            cout << "configuración" << endl;
        }

        if (msgType == "GET") {
            type = 2;
            cout << "recuperación" << endl;
        }

        if (msgType == "h") {
            type = 3;
            cout << "history" << endl;
        }

        std::cout << "Type: " << type << std::endl;
       
       
        
        /*
        const Value& v(int[]);
        v. = document["comdValues"].GetArray();
        StringBuffer bf2;
        PrettyWriter<StringBuffer> writer2(bf2);
        v.Accept(writer2);
        const char* json = bf2.GetString();
        */

        StringBuffer bf;
        PrettyWriter<StringBuffer> writer(bf);
        document.Accept(writer);

        std::cout << "A: "<< bf.GetString() << std::endl;


        //printf("%s\n", document["comdValues"].GetString());



        const Value& a = document["comdValues"].GetArray();
        assert(a.IsArray());
        
        
        StringBuffer bf2;
        Writer<StringBuffer> writer2(bf2);
        a.Accept(writer2);



        const char* json = bf2.GetString();

        std::cout << "B: " << bf2.GetString() << std::endl;


       

        
        GT::RCommand r = {
            10020,
            type,
            Info.client,
            "",
            "",
            "",
            "",//name
            (int)document["unitId"].GetInt(),
            document["commandId"].GetInt(),
            document["mode"].GetInt(),
            "",// date
            (unsigned short)document["level"].GetInt(),
            0,//index
            ClientMsg::Request,
            0,// time
            0// Delay
        };


        strcpy(r.user, document["user"].GetString());

        //unsigned int tag = db->getTag(document["unitId"].GetInt(), document["commandId"].GetInt(), type);
        //r.index = tag;
        /*std::string str = db->createCommand(
            (unsigned int)document["unitId"].GetInt(),
            (unsigned short)document["commandId"].GetInt(),
            to_string(tag), params, type);
            */
        std::string str = db->createCommand(&r, params);

        strcpy(r.message, str.c_str());
        cout << endl << "Unidad" <<  r.unit << endl << "COMANDO " << str << endl << endl;
       
        //db->addPending(document["unitId"].GetInt(), document["commandId"].GetInt(), tag, str, "pepe", type, (unsigned short)document["level"].GetInt());
        db->addPending(&r);
        
        strcpy(r.message, str.c_str());
        char buffer2[1024];
        memcpy(buffer2, &r, sizeof(r));
        send(s, buffer2, (int)sizeof(buffer2), 0);
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
        jsonResponse(Info.client, &response);
        /*

        encodeMessage((char*)"websocket 2021", buffer, size);

        printf("%s(%d)\n", Info.buffer, size);

        send(Info.client, buffer, (int)size, 0);
    */


    }

    void WebServer::send2(char* message) {
        char buffer[DEFAULT_BUFLEN];
        size_t size = 0;
        encodeMessage(message, buffer, size);

        //printf("%s(%d)\n", Info.buffer, size);

        //send(Info.client, buffer, (int)size, 0);
    }

    

}


void test1(void * app, char* buffer, size_t size) {

    GT::RCommand* x = (GT::RCommand*)buffer;


    cout << "TEST 1 Receiving ..." << endl;
    cout << "Buffer ..." << x->message << endl;
    GT::WebServer* WS = (GT::WebServer*)app;


    if (WS->getHeader(buffer) == 0) {
        cout << "ERROR" << endl;
    }

    
    
}

void test2(GT::CSInfo Info) {


    GT::RequestConnection c = {
        10001,
        Info.master,
        "hub1",
        "pepe",
        1,
        -2,
        "hola dejame entrar",
        0


    };
    char buffer2[512];
    memcpy(buffer2, &c, sizeof(c));
    send(Info.master, buffer2, sizeof(buffer2), 0);
    
    cout << "TEST DEBUG VALUE " << Info.master << endl;
    send(Info.master, "Barcelona vs Real Madrid", strlen("Barcelona vs Real Madrid"), 0);
}

BOOL __stdcall mainhub(LPVOID param) {
    GT::WebServer* WS = (GT::WebServer*)param;

    GT::CSInfo Info;
    Info.host = (char *)WS->configInit.hub.host;
    Info.port = WS->configInit.hub.port;// std::to_string(WS->configInit.hub.port).c_str();

    cout << "el puerto del hug es " << WS->configInit.hub.port << endl << endl;
    puts("Activate the HUB server");
    WS->hub = new GT::Hub(Info);
    WS->hub->appData = WS;
    WS->hub->CallConection = test2;
    WS->hub->callReceive = test1;
    cout << ANSI_COLOR_RESET "Asking for Connection..." << WS->configInit.port << endl;
    while (WS->reconnect) {
        WS->hub->start();
        cout << ANSI_COLOR_RESET "Reconnecting..."<< WS->configInit.port << endl;
        Sleep(5000);
    }

    cout << "Godd Bye" << endl;
    return true;
}

