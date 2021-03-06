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

const byte REQ_METHOD_SIZE       = 10;  // Max size of request method string
const byte REQ_PATH_SIZE         = 255; // Max size of request path string
const byte REQ_PARAMS_STR_SIZE   = 255; // Max size of query params string
const byte REQ_VERSION_SIZE      = 10;  // Max size of request version
const byte REQ_HEADER_COUNT      = 20;  // Max number of headers a request can hold
const byte REQ_HEADER_NAME_SIZE  = 50;  // Max size of header name
const byte REQ_HEADER_VALUE_SIZE = 255; // Max size of header value
const byte REQ_QUERY_PARAMS_SIZE = 10;  // Max number of query param objects in a single request
const int REQ_BODY_SIZE          = 2048;// Max size of body value

// HTTP Status codes - https://developer.mozilla.org/en-US/docs/Web/HTTP/Status

const char HTTP_OK[]                = "200 OK"; // Request succeeded
const char HTTP_BAD_REQUEST[]       = "400 Bad Request"; // The server could not understand the request due to invalid syntax.
const char HTTP_NOT_FOUND[]         = "404 Not Found"; // The server can not find the requested resource.
const char HTTP_LENGTH_REQUIRED[]   = "411 Length Required"; // Server rejected the request because the Content-Length header field is not defined and the server requires it.
const char HTTP_PAYLOAD_TOO_LARGE[] = "413 Payload Too Large"; // Request entity is larger than limits defined by server
const char HTTP_URI_TOO_LONG[]      = "414 URI Too Long"; // The URI requested by the client is longer than the server is willing to interpret.
const char HTTP_HEADER_TOO_LARGE[]  = "431 Request Header Fields Too Large"; // The server is unwilling to process the request because its header fields are too large.
const char HTTP_SERVER_ERROR[]      = "500 Internal Server Error"; // The server has encountered a situation it doesn't know how to handle.


// struct to hold HTTP Header key/value pairs
class HttpHeader {
    public:
        String key;
        String value;
};

// represents a request query parameter
struct QueryParam {
    String key; // param key
    String value; // param value
};

// Represents a response to an incoming HTTP request
class WebResponse {
    private:
        int _currentHeaderIndex = 0;
    public:
        WiFiClient client;
        String status;
        String httpVersion;
        String body;
        HttpHeader headers[REQ_HEADER_COUNT];

        int addHeader(HttpHeader); // add new header to header list
        int addHeader(const char*, const char*); // add new header to header list with basic strings (key, value)
        int addHeader(const char*, const long); // add numerical header, automatically parsing number to string
        int addHeader(const char*, const float); // add floating point numerical header, auto parsing to string

        int send(); // send the response to the client

};

// Represents an incoming HTTP request
class WebRequest {
    public:
        WiFiClient client;
        String method;
        String path;
        QueryParam params[REQ_QUERY_PARAMS_SIZE];
        String httpVersion;
        String body;
        HttpHeader headers[REQ_HEADER_COUNT];

        WebResponse getResponse();
        bool getHeader(String, HttpHeader&);
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

        int processIncomingRequest(WebRequest&); // process the next incoming request, return 1 when request found, -1 otherwise

        void readLine(WiFiClient); // read the next line into the internal _lineBuffer
        
        byte parseLineRequest(char*, char*, char*, char*); // parse buffered line as a request (src, method, path, params)
        byte parseLineHeader(char*, char*); // parse buffered line as a header (key, value)
        void parseQueryParams(char*, QueryParam*); // parse query params into provided array
};

#endif