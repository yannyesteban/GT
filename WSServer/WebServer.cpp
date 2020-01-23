#include "WebServer.h"
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
        
        puts("***************");
        puts(x);
        puts("***************");
        printf("[%s]\n", x);
        SOCKET s = hub->getHost();

        send(s, x, (int)sizeof(x), 0);

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

