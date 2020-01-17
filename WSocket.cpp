#include "WSocket.h"



namespace GT {
    WSocket::WSocket(SocketInfo pInfo):Socket(pInfo) {



	}
    WSocket::~WSocket() {
	}
    void WSocket::onConnect(ConnInfo Info) {
        puts("onConnect");
    }

    void WSocket::onMessage(ConnInfo Info) {
        puts("_CallMessage");

        if (!handshake(Info)) {
            message(Info);
        }
    }

    void WSocket::onClose(ConnInfo Info) {
        puts("onClose");
    }
    

    bool WSocket::handshake(ConnInfo Info) {

        int iSendResult;
        //char recvbuf[DEFAULT_BUFLEN];
        //int recvbuflen = DEFAULT_BUFLEN;
        //recvbuf = Info.buffer;
        Info.buffer[Info.valread] = 0;
       
        char sendbuf[1024];
        size_t sendbuf_size = 0;
        // see if it's requesting a key
        char* pKey = strstr(Info.buffer, "Sec-WebSocket-Key:");
        //printf("recibiendo:\n\n%s\n\n", Info.buffer);
        if (pKey) {

            //printf("Hola INIT \r\n");
            // parse just the key part
            pKey = strchr(pKey, ' ') + 1;
            char* pEnd = strchr(pKey, '\r');
            *pEnd = 0;

            char key[256];
            _snprintf_s(key, _countof(key), "%s%s", pKey, WEBSOCKET_KEY);

            unsigned char result[20];
            const unsigned char* pSha1Key = sha1(key);

            // endian swap each of the 5 ints
            for (int i = 0; i < 5; i++) {
                for (int c = 0; c < 4; c++)
                    result[i * 4 + c] = pSha1Key[i * 4 + (4 - c - 1)];
            }

            pKey = base64_encode(result, 20);

            const char* pTemplateResponse = "HTTP/1.1 101 Switching Protocols\r\n"
                "Upgrade: websocket\r\n"
                "Connection: Upgrade\r\n"
                "Sec-WebSocket-Accept: %s\r\n\r\n";

            _snprintf_s(sendbuf, _countof(sendbuf), pTemplateResponse, pKey);
            sendbuf_size = strlen(sendbuf);

            iSendResult = send(Info.client, sendbuf, (int)sendbuf_size, 0);
            return true;
        }

        return false;
    }

    bool WSocket::message(ConnInfo Info) {
        puts("_CallMessage");


        const char* ssss = decodeMessage(Info);

        printf("%s\n\n-----------------", ssss);


        //char message[] = Info.buffer;
        char buffer[DEFAULT_BUFLEN];
        size_t size=0;
        encodeMessage((char *)"QUEEEE", buffer, size);

        printf("%s(%d)\n\n-----------------", Info.buffer, size);

        send(Info.client, buffer, (int)size, 0);
        
        return true;
    }

    

    const char* WSocket::decodeMessage(ConnInfo Info) {
        Info.buffer[Info.valread] = 0;
        
        _websocket_header* h = (_websocket_header*)Info.buffer;

        _mask_key* mask_key;

        unsigned long long length;

        if (h->len < 126) {
            length = h->len;
            mask_key = (_mask_key*)(Info.buffer + sizeof(_websocket_header));
        } else if (h->len == 126) {
            _extended_16* extended = (_extended_16*)(Info.buffer + sizeof(_websocket_header));

            length = (extended->value[0] << 8) | extended->value[1];
            mask_key = (_mask_key*)(Info.buffer + sizeof(_websocket_header) + sizeof(_extended_16));
        } else {
            _extended_64* extended = (_extended_64*)(Info.buffer + sizeof(_websocket_header));

            length = (((unsigned long long) extended->value[0]) << 56)
                | (((unsigned long long) extended->value[1]) << 48)
                | (((unsigned long long) extended->value[2]) << 40)
                | (((unsigned long long) extended->value[3]) << 32)
                | (((unsigned long long) extended->value[4]) << 24)
                | (((unsigned long long) extended->value[5]) << 16)
                | (((unsigned long long) extended->value[6]) << 8)
                | (((unsigned long long) extended->value[7]) << 0);

            mask_key = (_mask_key*)(Info.buffer + sizeof(_websocket_header) + sizeof(_extended_64));
        }

        char* client_msg = ((char*)mask_key) + sizeof(_mask_key);

        if (h->mask) {
            for (int i = 0; i < length; i++) {
                client_msg[i] = client_msg[i] ^ mask_key->value[i % 4];
            }

        }

        return client_msg;
    }

    void WSocket::encodeMessage(char * message, char * sendbuf, size_t & sendbuf_size) {

        sendbuf_size = 0;

        _websocket_header* h = (_websocket_header*)sendbuf;

        unsigned long long length;

        char* pData;

        *h = _websocket_header{};

        h->opcode = 0x1; //0x1 = text, 0x2 = blob
        h->fin = 1;

        char text[DEFAULT_BUFLEN];

        _snprintf_s(text, _countof(text), "%s", message);

        unsigned long long msg_length = strlen(text);

        sendbuf_size = sizeof(_websocket_header);
        if (msg_length <= 125) {
            pData = sendbuf + sizeof(_websocket_header);
            h->len = msg_length;
        } else if (msg_length <= 0xffff) {
            h->len = 126;

            _extended_16* extended = (_extended_16*)(sendbuf + sendbuf_size);
            sendbuf_size += sizeof(_extended_16);

            extended->value[0] = (msg_length >> 8) & 0xff;
            extended->value[1] = msg_length & 0xff;
        } else {
            h->len = 127;

            _extended_64* extended = (_extended_64*)(sendbuf + sendbuf_size);
            sendbuf_size += sizeof(_extended_64);

            extended->value[0] = ((msg_length >> 56) & 0xff);
            extended->value[1] = ((msg_length >> 48) & 0xff);
            extended->value[2] = ((msg_length >> 40) & 0xff);
            extended->value[3] = ((msg_length >> 32) & 0xff);
            extended->value[4] = ((msg_length >> 24) & 0xff);
            extended->value[5] = ((msg_length >> 16) & 0xff);
            extended->value[6] = ((msg_length >> 8) & 0xff);
            extended->value[7] = ((msg_length >> 0) & 0xff);
        }

        pData = sendbuf + sendbuf_size;

        memcpy(pData, text, (size_t)msg_length);
        sendbuf_size += (size_t)msg_length;

    }

 

}