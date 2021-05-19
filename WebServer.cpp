
#include "Arduino.h"
#include "WebServer.h"
#include "Regexp.h"
#include "WiFiNINA.h"


// constructors 

WebServer::WebServer() : _server(WS_DEFAULT_PORT) { };

WebServer::WebServer(byte port) : _server(port) { };

// public methods

void WebServer::listen() {
    _server.begin();
};

void WebServer::processIncomingRequests() {
    // get incoming client requests
    WiFiClient client = _server.available();
    WebRequest req;

    // reset _lineMode
    _lineMode = LINE_MODE_REQUEST;

    if(client) { // ensure a client is connected
        Serial.println("WebServer - processing new request: ");
        int i = 0;
        int headerIndex = 0;
        int contentLength = 0;

        while(client.connected() && i < 50) { // ensure connection still open
            if(client.available()) { // ensure client stream is available
                // parse request line
                if(_lineMode == LINE_MODE_REQUEST) {
                    //read a new line into the buffer
                    readLine(client);

                    Serial.println("Processing request line...");
                    char method[REQ_METHOD_SIZE];
                    char path[REQ_PATH_SIZE];
                    char httpVersion[REQ_VERSION_SIZE];

                    // process request line
                    byte res = parseLineRequest(method, path, httpVersion);

                    if(res == PARSE_SUCCESS) {
                        //print results
                        Serial.println("Success!!!");
                        Serial.print("Method: "); Serial.println(method);
                        Serial.print("Path: "); Serial.println(path);
                        Serial.print("Version: "); Serial.println(httpVersion);

                        //assign values on request
                        req.method = method;
                        req.path = path;
                        req.httpVersion = httpVersion;
                    } else {
                        Serial.println("FAILED");
                    }
                    
                    //set line mode to headers for next iteration
                    _lineMode = LINE_MODE_HEADER;

                // parse header line
                } else if (_lineMode == LINE_MODE_HEADER) {
                    //read a new line into the buffer
                    readLine(client);

                    Serial.println("Processing header line...");
                    char headerKey[REQ_HEADER_NAME_SIZE];
                    char headerValue[REQ_HEADER_VALUE_SIZE];

                    // if blank line, switch to body mode and continue loop
                    if(_lineBuffer[0] == '\r') {
                        Serial.println("Empty line, switching to body mode");
                        _lineMode = LINE_MODE_BODY;
                        continue;
                    }

                    //parse line
                    parseLineHeader(headerKey, headerValue);
                    
                    //add header to request
                    HttpHeader h;
                    h.key = headerKey;
                    h.value = headerValue;
                    req.headers[headerIndex] = h;

                    // check for content length header and update local var
                    if(strcmp(headerKey, "Content-Length") == 0) {
                        contentLength = h.value.toInt();
                        Serial.print("Found Content-Length header: ");
                        Serial.println(contentLength);
                    }

                    //print parsed value
                    Serial.print("Added header - "); Serial.print(h.key); Serial.print(": "); Serial.println(h.value);

                //parse body line
                } else if (_lineMode == LINE_MODE_BODY) {
                    Serial.println("Processing body line...");
                    char body[REQ_BODY_SIZE];

                    client.readBytes(body, contentLength);

                    req.body = String(body);

                    Serial.println(req.body);
                } // end if (_lineMode == ...)

            } //end if (client.available())

            // increment loop counter so we can break out after x iterations.
            i++;

        } //end while(client.connected())

        //end connection with client after short delay
        delay(10);
        client.stop();
    } //end if (client)
};

// clear line buffer and read next line
void WebServer::readLine(WiFiClient client) {
    memset(_lineBuffer, 0, WS_LINE_BUFFER_SIZE);
    client.readBytesUntil(WS_LINE_TERMINATOR, _lineBuffer, WS_LINE_BUFFER_SIZE);
}

// parse the HTTP method and path from the current line in _lineBuffer
byte WebServer::parseLineRequest(char * method, char * path, char * version) {
    // check matches
    MatchState ms;
    ms.Target(_lineBuffer);
    char res = ms.Match("^(%u-) (%S-) (HTTP.*)");

    // process results
    switch(res) {
        case REGEXP_MATCHED: //match
            { // enclosing scope for variables created in this branch of the switch
                int matchCount = ms.level;
                if(matchCount != 3) { // unexpected number of matches
                    Serial.print("Unexpected number of matches when parsing request line: expected >= 2, actual = ");
                    Serial.println(matchCount);
                    break;
                }
                
                //get captured groups
                ms.GetCapture(method, 0);
                ms.GetCapture(path, 1);
                ms.GetCapture(version, 2);
                return PARSE_SUCCESS;
            }
            break;
        case REGEXP_NOMATCH: //no match
            Serial.print("No matches found...");
            break;
        default: //some sort of error
            Serial.print("Error trying to match...");
    }

    // if we exit the switch statement before returning, that means there was a problem parsing.
    return PARSE_FAIL;
};

// parse an HTTP header from the current line in _lineBuffer
byte WebServer::parseLineHeader(char * key, char * value) {
    MatchState ms;
    ms.Target(_lineBuffer);
    char res = ms.Match("^(.-): (.*)");
    
    switch(res) {
        case REGEXP_MATCHED:
            {
                int matchCount = ms.level;
                if(matchCount != 2) {
                    Serial.print("Unexpected number of matches when parsing header line: expected = 2, actual = ");
                    Serial.println(matchCount);
                    break;
                }

                //get captured groups
                ms.GetCapture(key, 0);
                ms.GetCapture(value, 1);
            }
            break;
        case REGEXP_NOMATCH:
            Serial.println("No matches found...");
            break;
        default: //some sort of error
            Serial.print("Error trying to match...");
    }

    // if we exit the switch statement before returning, that means there was a problem parsing.
    return PARSE_FAIL;
}