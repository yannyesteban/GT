#define _CRT_SECURE_NO_WARNINGS
#include "WebServer.h"
using namespace rapidjson;

namespace GT {
    WebServer::WebServer(SocketInfo pInfo):
        WebSocketServer(pInfo),
        
        hub(nullptr) {
    }
    void WebServer::init() {

        hClientThread = CreateThread(
            NULL,
            0,
            (LPTHREAD_START_ROUTINE)mainhub,
            this,
            0,
            &dwThreadId);


        
       
	}

    void WebServer::onMessage(ConnInfo Info) {
        WebSocketServer::onMessage(Info);
        
        const char* x = decodeMessage(Info);
        
        puts("===============");
        puts(x);
        puts("***************");
        printf("[%s]\n", x);
        SOCKET s = hub->getHost();

        Document document;
        document.Parse(x);
        if (!document.IsObject()) {
            printf("ERROR JSON...!!!\n");
            return;
        }
        printf("JSON deviceId %d\n", document["deviceId"].GetInt());
        
        CMDMsg msg = {
            10010,
            1,

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

        std::cout << "B: " << bf2.GetString() << std::endl;


        strcpy(msg.deviceName, document["deviceName"].GetString());
        strcpy(msg.params, bf2.GetString());
        //n = strlen(&szBuff[0]);
        char buffer2[255];
        memcpy(buffer2, &msg, sizeof(msg));
        //Info.valread = sizeof(xx);
        //send(s, x, (int)sizeof(x), 0);
        send(s, buffer2, (int)sizeof(buffer2), 0);
        printf("BUFFER2 es : %s, %d, %d\n", buffer2, sizeof(buffer2), sizeof(msg));
        printf("---------------\n");
        //msg.params = (char *)document["msg"].GetString();
        //strncpy(msg.params, document["msg"].GetString(), document["msg"].GetStringLength());
        printf("%s\n", msg.params);
        //printf("%s\n", msg.p);

    }

    

}


BOOL __stdcall mainhub(LPVOID param) {
    GT::WebServer* WS = (GT::WebServer*)param;

    GT::CSInfo Info;
    Info.host = (char*)"127.0.0.1";
    Info.port = "3311";
    puts("hola a servidor");
    WS->hub = new GT::Hub(Info);
    WS->hub->start();

    return true;
}

