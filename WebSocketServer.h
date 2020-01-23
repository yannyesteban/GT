#pragma once
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Socket.h"
#include "sha1.h"

#define WEBSOCKET_KEY   "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define DEFAULT_BUFLEN  512
#define DEFAULT_PORT    "8083"


#pragma pack (push, 1)
struct _websocket_header {
	unsigned char opcode : 4;

	unsigned char rsv3 : 1;
	unsigned char rsv2 : 1;
	unsigned char rsv1 : 1;
	unsigned char fin : 1;

	unsigned char len : 7;
	unsigned char mask : 1;
};

struct _extended_16 {
	unsigned char value[2];
};

struct _extended_64 {
	unsigned char value[8];
};

struct _mask_key {
	unsigned char value[4];
};
#pragma pack (pop)


namespace GT {
	class WebSocketServer : public Socket {
	public:
		WebSocketServer(SocketInfo pInfo);
		~WebSocketServer();


		bool handshake(ConnInfo Info);
		bool message(ConnInfo Info);


		virtual void onConnect(ConnInfo Info);
		virtual void onMessage(ConnInfo Info);
		virtual void onClose(ConnInfo Info);

		virtual const char * decodeMessage(ConnInfo Info);
		virtual void encodeMessage(char* message, char * buffer, size_t & size);
	};

};