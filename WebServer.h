#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "WiFiNINA.h"

const byte WEB_SERVER_DEFAULT_PORT = 80;

class WebServer {
    private:
        WiFiServer _server;
        byte _port;
    public:
        WebServer();
        WebServer(byte);

        void listen();
        void processIncomingRequests();
};

#endif