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

    void WebServer::onMessage(ConnInfo Info) {


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
            type = 1;
            cout << "connecting" << endl;

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
        for (rapidjson::Value::ConstValueIterator itr = values.Begin(); itr != values.End(); ++itr) {
            const rapidjson::Value& attribute = *itr;
            assert(attribute.IsObject()); // each attribute is an object
            for (rapidjson::Value::ConstMemberIterator itr2 = attribute.MemberBegin(); itr2 != attribute.MemberEnd(); ++itr2) {
                //std::cout << itr2->name.GetString() << " : " << std::endl;
                std::cout << itr2->name.GetString() << " : " << itr2->value.GetString() << std::endl;
            }
        }


        for (SizeType i = 0; i < values.Size(); i++) {
            std::cout << "         hooolaaaaaaaaa " << std::endl;
            //params.push_back(values[i].GetString());
            //std::cout << " Array " << i << " es " << values[i].GetString() << std::endl;
        }

        for (std::list<std::string>::iterator it = params.begin() ; it != params.end(); ++it) {
            std::cout << " Array \t" << *it <<  std::endl;
        }
        
        //cout << values[0].GetString() << endl;




        if (msgType == "set") {
            type = 1;
            cout << "configuración" << endl;
        }

        if (msgType == "get") {
            type = 2;
            cout << "recuperación" << endl;
        }

        if (msgType == "h") {
            type = 3;
            cout << "history" << endl;
        }

        std::cout << "Type: " << type << std::endl;
       
        CMDMsg msg = {
            10010,
            type,
            (unsigned short)document["unitId"].GetInt(),
            (unsigned short)document["deviceId"].GetInt(),
            "2012000066",
            (unsigned short)document["commandId"].GetInt(),

            "TAG1",
            "0000",
            ""// document["msg"].GetString()
        };
        
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

        //std::cout << "B: " << bf2.GetString() << std::endl;


        strcpy(msg.deviceName, document["deviceName"].GetString());
        strcpy(msg.params, bf2.GetString());
        //n = strlen(&szBuff[0]);
        char buffer2[255];
        memcpy(buffer2, &msg, sizeof(msg));
        //Info.valread = sizeof(xx);
        //send(s, x, (int)sizeof(x), 0);


        GT::RCommand r = {
            10020,
            1,
            "pepe",
            "",
            "2012000750",
            4737
        };
        
        unsigned int tag = db->getTag(document["unitId"].GetInt(), document["commandId"].GetInt());
        
        std::string str = db->createCommand(
            (unsigned int)document["unitId"].GetInt(),
            (unsigned short)document["commandId"].GetInt(),
            to_string(tag), params, type);

        cout << endl << endl << "COMANDO " << str << endl << endl;
        db->addPending(document["unitId"].GetInt(), document["commandId"].GetInt(), tag, str, "pepe");
        strcpy(r.message, str.c_str());
        memcpy(buffer2, &r, sizeof(r));
        send(s, buffer2, (int)sizeof(buffer2), 0);
        //send(Info.client, "yanny", strlen("yanny"), 0);

        char buffer[DEFAULT_BUFLEN];
        size_t size = 0;
        encodeMessage((char*)"QUEEEE", buffer, size);

        printf("%s(%d)\n", Info.buffer, size);

        send(Info.client, buffer, (int)size, 0);


        //printf("BUFFER2 es : %s, %d, %d\n", buffer2, sizeof(buffer2), sizeof(msg));
        //printf("---------------\n");
        //msg.params = (char *)document["msg"].GetString();
        //strncpy(msg.params, document["msg"].GetString(), document["msg"].GetStringLength());
        //printf("%s\n", msg.params);
        //printf("%s\n", msg.p);

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
    GT::WebServer* WS = (GT::WebServer*)app;
    std::cout << "Token " << WS->Token << std::endl;
    std::cout << "uno " << std::endl;
    std::cout << "que " << buffer << std::endl;
}

BOOL __stdcall mainhub(LPVOID param) {
    GT::WebServer* WS = (GT::WebServer*)param;

    GT::CSInfo Info;
    Info.host = (char*)"127.0.0.1";
    Info.port = "3320";
    puts("Activate the HUB server");
    WS->hub = new GT::Hub(Info);
    WS->hub->appData = WS;
    WS->hub->callReceive = test1;
    WS->hub->start();

    return true;
}

