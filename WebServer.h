#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "WiFiNINA.h"

// Server constants

const byte WS_DEFAULT_PORT     = 80;      // default to port 80
const int WS_LINE_BUFFER_SIZE = 512; // line buffer can hold up to 512 chars.
const char WS_LINE_TERMINATOR  = '\n'; // line terminator character

// line mode status constants, determine how to process current line when parsing requests.

const byte LINE_MODE_REQUEST = 0; //Current line is a request
const byte LINE_MODE_HEADER  = 1; //Current line is a header
const byte LINE_MODE_BODY    = 2; //Current line is a body

// parser status constants

const byte PARSE_SUCCESS = 0; // Parsing succeeded
const byte PARSE_FAIL    = 1; // Parsing failed

// data size constants - since I need to use char[] for these, I must define a fixed max size for each char[]

const byte REQ_METHOD_SIZE       = 10;  // Max size of request method
const byte REQ_PATH_SIZE         = 255; // Max size of request path
const byte REQ_VERSION_SIZE      = 10;  // Max size of request version
const byte REQ_HEADER_COUNT      = 20;  // Max number of headers a request can hold
const byte REQ_HEADER_NAME_SIZE  = 50;  // Max size of header name
const byte REQ_HEADER_VALUE_SIZE = 255; // Max size of header value
const int REQ_BODY_SIZE          = 2048;// Max size of body value

// struct to hold HTTP Header key/value pairs
struct HttpHeader {
    String key;
    String value;
};

// struct to hold web request details
struct WebRequest {
    String method;
    String path;
    String httpVersion;
    String body;
    HttpHeader headers[REQ_HEADER_COUNT];
};

// Web Server class, provides methods for processing and replying to incoming requests.
class WebServer {
    private:
        WiFiServer _server; // WiFi server instance
        byte _lineMode;
        char _lineBuffer[WS_LINE_BUFFER_SIZE];
    public:
        WebServer();
        WebServer(byte);

        void listen(); // begin listening for incoming requests

        void processIncomingRequests(); // process the next incoming request
        
        void readLine(WiFiClient); // read the next line into the internal _lineBuffer

        byte parseLineRequest(char*, char*, char*); // parse buffered line as a request (src, method, path)
        byte parseLineHeader(char*, char*); // parse buffered line as a header (key, value)
};

#endif