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


        auto appInfo = GT::Config::load("config.json");

        cout << appInfo.db.name << endl;
        //config = pConfig;
        // pConfig.db.debug = pConfig.debug;
        db = new DB(appInfo.db);
        db->connect();


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
        char buffer[100];
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
                char buffer2[255];
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
            char buffer2[100];
            size_t size2;
           encodeMessage((char*)str.c_str(), buffer2, size2);

            send(response->id, buffer2, size2, 0);
            //cout << "jamas" << endl;
            return 1;
        }


        return 0;
    }

    void WebServer::jsonResponse(SOCKET client, RCommand * response) {
        Document z;
        z.SetObject();
       

        Value msg;
        char buffer[255];
        int len = sprintf(buffer, "%s", response->message);
        msg.SetString(response->message, strlen(response->message), z.GetAllocator());
        z.AddMember("message", msg, z.GetAllocator());
        z.AddMember("lastname", "Esteban", z.GetAllocator());
        z["lastname"].SetString(response->user, strlen(response->user), z.GetAllocator());
      
            

        StringBuffer bf5;
        Writer<StringBuffer> writer5(bf5);

        z.Parse(bf5.GetString());

        z.Accept(writer5);

        std::cout << "A5: " << bf5.GetString() << std::endl;



        string str = response->message;
        char buffer2[255];
        size_t size2;
        encodeMessage((char*)bf5.GetString(), buffer2, size2);

        send(client, buffer2, size2, 0);
    
    }

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
            1,
            Info.client,
            "",
            "",
            "",
            (int)document["unitId"].GetInt(),
            document["commandId"].GetInt(),
            document["mode"].GetInt(),
            (unsigned short)document["level"].GetInt(),
            0
        };
        strcpy(r.user, document["user"].GetString());

        unsigned int tag = db->getTag(document["unitId"].GetInt(), document["commandId"].GetInt());
        r.index = tag;
        std::string str = db->createCommand(
            (unsigned int)document["unitId"].GetInt(),
            (unsigned short)document["commandId"].GetInt(),
            to_string(tag), params, type);

        strcpy(r.message, str.c_str());
        cout << endl << endl << "COMANDO " << str << endl << endl;
       
        //db->addPending(document["unitId"].GetInt(), document["commandId"].GetInt(), tag, str, "pepe", type, (unsigned short)document["level"].GetInt());
        db->addPending(&r);
        
        strcpy(r.message, str.c_str());
        char buffer2[255];
        memcpy(buffer2, &r, sizeof(r));
        send(s, buffer2, (int)sizeof(buffer2), 0);
        //send(Info.client, "yanny", strlen("yanny"), 0);

        char buffer[DEFAULT_BUFLEN];
        size_t size = 0;
        
        encodeMessage((char*)"websocket 2021", buffer, size);

        printf("%s(%d)\n", Info.buffer, size);

        send(Info.client, buffer, (int)size, 0);



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
    char buffer2[255];
    memcpy(buffer2, &c, sizeof(c));
    send(Info.master, buffer2, sizeof(buffer2), 0);

    cout << "YESSSSSSSSSSS " << Info.master << endl;
    send(Info.master, "Barcelona vs Real Madrid", strlen("Barcelona vs Real Madrid"), 0);
}

BOOL __stdcall mainhub(LPVOID param) {
    GT::WebServer* WS = (GT::WebServer*)param;

    GT::CSInfo Info;
    Info.host = (char*)"127.0.0.1";
    Info.port = "3320";
    puts("Activate the HUB server");
    WS->hub = new GT::Hub(Info);
    WS->hub->appData = WS;
    WS->hub->CallConection = test2;
    WS->hub->callReceive = test1;
    WS->hub->start();

    return true;
}

