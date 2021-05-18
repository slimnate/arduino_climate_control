#include "WebServer.h"
#include "Arduino.h"
#include "WiFiNINA.h"

// constructors 

WebServer::WebServer() : _server(WEB_SERVER_DEFAULT_PORT), _port(WEB_SERVER_DEFAULT_PORT) { };

WebServer::WebServer(byte port) : _server(port), _port(port) { };

// public methods

void WebServer::listen() {
    _server.begin();
};

void WebServer::processIncomingRequests() {
    // incoming client request
    WiFiClient client = _server.available();

    if(client) {
        Serial.println("WebServer - processing new request: ");

        while(client.connected()) {
            if(client.available()) {
                char c = client.read();
                Serial.write(c);
            }
        }
    }
};