
#include "Arduino.h"
#include "WebServer.h"
#include "Regexp.h"
#include "WiFiNINA.h"


// ==== WebServer ====

//create WebServer with default port
WebServer::WebServer() : _server(WS_DEFAULT_PORT) { };

//create WebServer with custom port
WebServer::WebServer(byte port) : _server(port) { };

// begin listening to requests
void WebServer::listen() {
    _server.begin();
};

// process next incoming request, updating provided WebRequest object.
int WebServer::processIncomingRequest(WebRequest & req) {
    // get incoming client requests
    WiFiClient client = _server.available();

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
                    char params[REQ_PARAMS_STR_SIZE];

                    // process request line
                    byte res = parseLineRequest(method, path, params, httpVersion);

                    if(res == PARSE_SUCCESS) {
                        //print results
                        Serial.println("Success!!!");
                        Serial.print("Method: "); Serial.println(method);
                        Serial.print("Path: "); Serial.println(path);
                        Serial.print("Params string: "); Serial.println(params);
                        Serial.print("Version: "); Serial.println(httpVersion);

                        //assign values on request
                        req.method = method;
                        req.path = path;
                        req.httpVersion = httpVersion;
                        parseQueryParams(params, req.params);
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

                    // return the parsed request object, for external handling, make sure to add client first
                    req.client = client;
                    return 1;
                } // end if (_lineMode == ...)

            } //end if (client.available())

        } //end while(client.connected())

    } //end if (client)
    return -1; // return fail if no incoming requests
};

// clear line buffer and read next line
void WebServer::readLine(WiFiClient client) {
    memset(_lineBuffer, 0, WS_LINE_BUFFER_SIZE);
    client.readBytesUntil(WS_LINE_TERMINATOR, _lineBuffer, WS_LINE_BUFFER_SIZE);
}

// parse the HTTP method, path, and query param strings from the current line in _lineBuffer
byte WebServer::parseLineRequest(char * method, char * path, char * params, char* version) {
    // check matches
    MatchState ms;
    ms.Target(_lineBuffer);
    char res = ms.Match("^(%u-) (%S-)?(%S-) (HTTP.*)");

    // process results
    switch(res) {
        case REGEXP_MATCHED: //match
            { // enclosing scope for variables created in this branch of the switch
                int matchCount = ms.level;
                if(matchCount != 4) { // unexpected number of matches
                    Serial.print("Unexpected number of matches when parsing request line: expected = 4, actual = ");
                    Serial.println(matchCount);
                    break;
                }
                
                //get captured groups
                ms.GetCapture(method, 0);
                ms.GetCapture(path, 1);
                ms.GetCapture(params, 2);
                ms.GetCapture(version, 3);

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

void WebServer::parseQueryParams(char* paramStr, QueryParam* dest) {
    bool inKey = true;
    char keyBuffer[REQ_PARAMS_STR_SIZE];
    char valueBuffer[REQ_PARAMS_STR_SIZE];
    QueryParam paramBuffer;
    int bufferIndex = 0;
    int paramIndex = 0;

    //make sure buffers are empty to start
    memset(keyBuffer, 0, REQ_PARAMS_STR_SIZE);
    memset(valueBuffer, 0, REQ_PARAMS_STR_SIZE);

    Serial.println("Parsing query params");

    for(int i = 0; i < REQ_PARAMS_STR_SIZE; i++) {
        char c = paramStr[i];
        Serial.print("Current char: "); Serial.print(c);

        //return if reached end of string
        if(c == 0x00) {
            Serial.println("..end");
            //add last from buffer and return
            dest[paramIndex] = QueryParam { String(keyBuffer), String(valueBuffer) };
            return;
        }

        if(inKey) { // processing a key
            Serial.print("..in key");
            if(c == '=') {
                Serial.println("..end of key");
                // end of key
                inKey = false;
                bufferIndex = 0;
            } else {
                Serial.println("..add to buffer");
                //add char to keyBuffer, and increment bufferIndex
                keyBuffer[bufferIndex] = c;
                bufferIndex++;
            }
        } else { // processing a value
            Serial.print("..in value");
            if(c == '&') {
                Serial.println("..end of value");
                // end of value
                inKey = true;
                //add QueryParam object
                dest[paramIndex] = QueryParam { String(keyBuffer), String(valueBuffer) };

                //clear buffers
                memset(keyBuffer, 0x00, REQ_PARAMS_STR_SIZE);
                memset(valueBuffer, 0x00, REQ_PARAMS_STR_SIZE);

                //update indices
                paramIndex++;
                bufferIndex = 0;
            } else {
                Serial.println("..add to buffer");
                //add char to valueBuffer and increment bufferIndex
                valueBuffer[bufferIndex] = c;
                bufferIndex++;
            }
        }

    }
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
};

// ==== WebRequest ====

//return a WebResponse object that can be used to reply to the incoming request
WebResponse WebRequest::getResponse() {
    //create new response object
    WebResponse res;
    res.client = client;
    res.httpVersion="HTTP/1.1";

    //set up default status
    res.status = HTTP_OK;

    //set up default headers
    res.addHeader("Content-Type", "text/plain");
    res.addHeader("Server", "Arduino NANO 33 IoT - Snake Tank Controller");
    res.addHeader("Connection", "close");

    return res;
};


// ==== WebResponse ====

// Add a new header to the header list. Returns 1 for success, returns -1 if error.
int WebResponse::addHeader(char * key, char * value) {
    HttpHeader h;
    h.key = key;
    h.value = value;
    return addHeader(h);
};

// Add a new header to the header list. Returns 1 for success, returns -1 if error.
int WebResponse::addHeader(HttpHeader h) {
    //make sure we have room
    if(_currentHeaderIndex >= REQ_HEADER_COUNT) {
        return -1; //return fail
    }

    //add header
    headers[_currentHeaderIndex] = h;
    _currentHeaderIndex += 1;
    return 1; // return success
};

// Attempt to send the response to the requesting client. Returns -1 for fail, 1 for success.
int WebResponse::send() {
    if(client.connected()) {
        //send version and status line.
        client.println(httpVersion + " " + status);

        // calculate content-length header and add
        int bodyLen = body.length();
        if(bodyLen > 0) {
            char bls[5];
            itoa(bodyLen, bls, 10);
            addHeader("Content-Length", bls);
        } else {
            addHeader("Content-Length", "0");
        }

        //send headers
        for (int i = 0; i < _currentHeaderIndex; i++) {
            HttpHeader h = headers[i];
            client.println(h.key + ": " + h.value);
        }

        client.println(); // empty line to signify end of headers

        // send body
        client.print(body);

        // wait for client to receive all data
        delay(20);

        // close connection and return success
        client.stop();
        return 1;
    }

    return -1; // client not connected.
}